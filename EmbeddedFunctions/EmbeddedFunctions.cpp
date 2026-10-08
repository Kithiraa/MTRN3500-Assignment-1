#include "EmbeddedFunctions.h"

using namespace System;
using namespace System::Text;
using namespace System::Net::Sockets;


EmbeddedFunctions::EmbeddedFunctions()
{
    client = nullptr;
    stream = nullptr;
}


EmbeddedFunctions::~EmbeddedFunctions()
{
    if (stream != nullptr)
        stream->Close();

    if (client != nullptr)
        client->Close();
}


void EmbeddedFunctions::GOpen(String^ address, const int port)
{
    try
    {
        client = gcnew TcpClient();

        // Connect to Galil
        client->Connect(address, port);

        // Get TCP stream for reading/writing
        stream = client->GetStream();
    }
    catch (Exception^ e)
    {
        throw gcnew Exception(
            "Failed to open Galil connection: " + e->Message
        );
    }
}


void EmbeddedFunctions::GClose()
{
    try
    {
        if (stream != nullptr)
        {
            stream->Close();
            stream = nullptr;
        }

        if (client != nullptr)
        {
            client->Close();
            client = nullptr;
        }
    }
    catch (Exception^ e)
    {
        throw gcnew Exception(
            "Failed to close Galil connection: " + e->Message
        );
    }
}


String^ EmbeddedFunctions::GCommand(String^ command)
{
    try
    {
        if (stream == nullptr)
        {
            throw gcnew InvalidOperationException(
                "Galil connection is not open"
            );
        }

        // Add semicolon if missing
        if (!command->EndsWith(";"))
        {
            command += ";";
        }

        // Galil communication requires carriage return
        command += "\r";

        // Convert command string to ASCII bytes
        array<Byte>^ sendData =
            Encoding::ASCII->GetBytes(command);

        // Send command
        stream->Write(
            sendData,
            0,
            sendData->Length
        );

        // Buffer for Galil response
        array<Byte>^ receiveData =
            gcnew array<Byte>(2048);

        // Read response
        int bytesRead =
            stream->Read(
                receiveData,
                0,
                receiveData->Length
            );

        // Convert received bytes back to String
        String^ response =
            Encoding::ASCII->GetString(
                receiveData,
                0,
                bytesRead
            );

        return response;
    }
    catch (Exception^ e)
    {
        throw gcnew Exception(
            "Galil command failed: " + e->Message
        );
    }
}