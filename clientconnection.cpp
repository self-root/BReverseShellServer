#include "clientconnection.hpp"

#include <boost/bind.hpp>
#include <filesystem>

ClientConnection::ClientConnection(boost::asio::ip::tcp::socket &&socket_)
    : socket(std::move(socket_)), inputThread(&Input::get, &input)
{
    input.ready.connect([this](std::string s)
                        { this->onInputValue(s); });
}

ClientConnection::~ClientConnection()
{
}

void ClientConnection::onInputValue(std::string value)
{
    auto splitted = util::Utility::splitString(value, ' ');
    auto program = splitted.front();
    auto args = util::Utility::joinString(splitted, 1);

    if (program == "clr")
    {
        tempData.clear();
        return;
    }

    json jsonMessage;
    jsonMessage["program"] = program;
    jsonMessage["args"] = args;

    sendCommand(jsonMessage);
}

void ClientConnection::sendCommand(const json &res_)
{
    auto jsonResponse = json::to_bson(res_);

    boost::system::error_code err;

    socket.write_some(boost::asio::buffer(jsonResponse, jsonResponse.size()), err);

    if (err)
        std::cout << "Error while sending message: " << err.message() << std::endl;
}

void ClientConnection::startReceive()
{
    socket.async_read_some(
        boost::asio::buffer(bufferData, BUFFER_SIZE),
        boost::bind(
            &ClientConnection::handleDataReading,
            shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred
        )
    );
}

void ClientConnection::handleDataReading(const boost::system::error_code &err, std::size_t receivedBytes)
{
    if (!err)
    {
        tempData.insert(std::end(tempData), std::begin(bufferData), (std::begin(bufferData) + receivedBytes));

        auto s = socket.available();
        // An attempt to parse the data to json if there is no available data to read
        if (s == 0)
        {
            parse();
        }
    }

    else if (err == boost::asio::error::eof)
    {
        std::cout << "Host disconnected\n";
        socket.close();
    }

    else
    {
        std::cout << "ERROR: " << err.message() << std::endl;
        exit(1);
    }

    startReceive();
}

void ClientConnection::parse()
{
    try
    {
        json jsonData = json::from_bson(tempData);
        readJson(jsonData);
        tempData.clear();
    }
    catch (nlohmann::detail::parse_error &error)
    {
        std::cout << "Data is not completed or broken...\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

void ClientConnection::readJson(const json &j_)
{
    if (j_.contains("data"))
    {

        auto data = j_.at("data").get_binary();

        writeData(data, j_.at("res"));
    }
    std::string output = j_.at("res");
    std::string cwd = j_.at("cwd");

    input.setCurrentDir(cwd);
    std::cout << output << std::endl;
}

void ClientConnection::writeData(const nlohmann::byte_container_with_subtype<std::vector<uint8_t>> &data,
                                 const std::string &filename)
{
    std::filesystem::path destPath("serverfiles");

    if (!std::filesystem::exists(destPath))
        std::filesystem::create_directory(destPath);

    std::string filePath(std::string(destPath.c_str()) + "/" + filename);
    auto outfile = std::ofstream(filePath.c_str(), std::ios::binary);

    outfile.write((const char *)&data[0], data.size());

    std::cout << "Data saved: " << filename << '\n';
}