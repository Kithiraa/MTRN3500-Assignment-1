#include "Galil.h"

#include <sstream>
#include <iomanip>
#include <stdexcept>


constexpr bool USE_SIMULATOR = true;


// =========================================================
// DEFAULT CONSTRUCTOR
// =========================================================

Galil::Galil()
    : Functions(new EmbeddedFunctions(USE_SIMULATOR)),
    g(0),
    ControlParameters{ 1.0, 1.0, 1.0 },
    setPoint(0),
    ownsFunctions(true),
    addr("192.168.0.200"),
    lastWriteResult(G_NO_ERROR),
    lastWriteResponse("")
{
    GReturn result = Functions->GOpen(addr.c_str(), &g);

    if (result != G_NO_ERROR)
    {
        delete Functions;
        Functions = nullptr;

        CheckGReturn(result);
    }
}


// =========================================================
// CONSTRUCTOR WITH SUPPLIED EMBEDDED FUNCTIONS
// =========================================================

Galil::Galil(EmbeddedFunctions* Funcs, GCStringIn address)
    : Functions(Funcs),
    g(0),
    ControlParameters{ 1.0, 1.0, 1.0 },
    setPoint(0),
    ownsFunctions(false),
    addr(address),
    lastWriteResult(G_NO_ERROR),
    lastWriteResponse("")
{
    CheckGReturn(
        Functions->GOpen(addr.c_str(), &g)
    );
}


// =========================================================
// COPY CONSTRUCTOR
// =========================================================

Galil::Galil(const Galil& other)
    : Functions(new EmbeddedFunctions(USE_SIMULATOR)),
    g(0),
    ControlParameters{
        other.ControlParameters[0],
        other.ControlParameters[1],
        other.ControlParameters[2]
    },
    setPoint(other.setPoint),
    ownsFunctions(true),
    addr(other.addr),
    lastWriteResult(G_NO_ERROR),
    lastWriteResponse("")
{
    GReturn result = Functions->GOpen(addr.c_str(), &g);

    if (result != G_NO_ERROR)
    {
        delete Functions;
        Functions = nullptr;

        CheckGReturn(result);
    }
}


// =========================================================
// DESTRUCTOR
// =========================================================

Galil::~Galil()
{
    if (Functions != nullptr)
    {
        // Do not throw exceptions from destructor
        if (g != 0)
            Functions->GClose(g);

        if (ownsFunctions)
            delete Functions;
    }
}


// =========================================================
// DIGITAL OUTPUTS
// =========================================================

void Galil::DigitalOutput(uint16_t value)
{
    uint16_t lower = value & 0xFF;
    uint16_t upper = value >> 8;

    std::string command =
        "OP " + std::to_string(lower) + "," +
        std::to_string(upper) + ";";

    char buffer[1024] = {};
    GSize bytesReturned = 0;

    lastWriteResult = Functions->GCommand(
        g,
        command.c_str(),
        buffer,
        sizeof(buffer),
        &bytesReturned
    );

    lastWriteResponse = buffer;
}


// =========================================================

void Galil::DigitalByteOutput(bool bank, uint8_t value)
{
    uint16_t lower = 0;
    uint16_t upper = 0;

    char buffer[1024] = {};
    GSize bytesReturned = 0;

    if (bank == 0)
    {
        // Set low bank to requested value
        lower = value;

        // Preserve current high bank
        for (uint8_t bit = 8; bit < 16; bit++)
        {
            std::string query =
                "MG @OUT[" + std::to_string(bit) + "];";

            buffer[0] = '\0';

            CheckGReturn(
                Functions->GCommand(
                    g,
                    query.c_str(),
                    buffer,
                    sizeof(buffer),
                    &bytesReturned
                )
            );

            if (std::stoi(buffer) != 0)
            {
                upper |= static_cast<uint16_t>(
                    1u << (bit - 8)
                    );
            }
        }
    }
    else
    {
        // Set high bank to requested value
        upper = value;

        // Preserve current low bank
        for (uint8_t bit = 0; bit < 8; bit++)
        {
            std::string query =
                "MG @OUT[" + std::to_string(bit) + "];";

            buffer[0] = '\0';

            CheckGReturn(
                Functions->GCommand(
                    g,
                    query.c_str(),
                    buffer,
                    sizeof(buffer),
                    &bytesReturned
                )
            );

            if (std::stoi(buffer) != 0)
            {
                lower |= static_cast<uint16_t>(
                    1u << bit
                    );
            }
        }
    }

    std::string command =
        "OP " + std::to_string(lower) + "," +
        std::to_string(upper) + ";";

    buffer[0] = '\0';

    lastWriteResult = Functions->GCommand(
        g,
        command.c_str(),
        buffer,
        sizeof(buffer),
        &bytesReturned
    );

    lastWriteResponse = buffer;
}


// =========================================================

void Galil::DigitalBitOutput(bool val, uint8_t bit)
{
    std::string command;

    if (val)
        command = "SB " + std::to_string(bit) + ";";
    else
        command = "CB " + std::to_string(bit) + ";";

    char buffer[1024] = {};
    GSize bytesReturned = 0;

    lastWriteResult = Functions->GCommand(
        g,
        command.c_str(),
        buffer,
        sizeof(buffer),
        &bytesReturned
    );

    lastWriteResponse = buffer;
}


// =========================================================
// DIGITAL INPUTS
// =========================================================

uint16_t Galil::DigitalInput()
{
    uint16_t value = 0;

    for (uint8_t bit = 0; bit < 16; bit++)
    {
        if (DigitalBitInput(bit))
        {
            value |= static_cast<uint16_t>(
                1u << bit
                );
        }
    }

    return value;
}


// =========================================================

uint8_t Galil::DigitalByteInput(bool bank)
{
    uint16_t value = DigitalInput();

    if (bank == 0)
    {
        return static_cast<uint8_t>(
            value & 0xFF
            );
    }
    else
    {
        return static_cast<uint8_t>(
            value >> 8
            );
    }
}


// =========================================================

bool Galil::DigitalBitInput(uint8_t bit)
{
    std::string command =
        "MG @IN[" + std::to_string(bit) + "];";

    char buffer[1024] = {};
    GSize bytesReturned = 0;

    CheckGReturn(
        Functions->GCommand(
            g,
            command.c_str(),
            buffer,
            sizeof(buffer),
            &bytesReturned
        )
    );

    return std::stoi(buffer) != 0;
}


// =========================================================
// WRITE CHECK
// =========================================================

bool Galil::CheckSuccessfulWrite()
{
    return
        lastWriteResult == G_NO_ERROR &&
        lastWriteResponse.find('?') == std::string::npos;
}


// =========================================================
// ANALOG FUNCTIONS
// =========================================================

float Galil::AnalogInput(uint8_t channel)
{
    std::string command =
        "MG @AN[" + std::to_string(channel) + "];";

    char buffer[1024] = {};
    GSize bytesReturned = 0;

    CheckGReturn(
        Functions->GCommand(
            g,
            command.c_str(),
            buffer,
            sizeof(buffer),
            &bytesReturned
        )
    );

    return std::stof(buffer);
}


// =========================================================

void Galil::AnalogOutput(uint8_t channel, double voltage)
{
    std::ostringstream voltageStream;

    voltageStream
        << std::fixed
        << std::setprecision(2)
        << voltage;

    std::string command =
        "AO " + std::to_string(channel) + "," +
        voltageStream.str() + ";";

    char buffer[1024] = {};
    GSize bytesReturned = 0;

    lastWriteResult = Functions->GCommand(
        g,
        command.c_str(),
        buffer,
        sizeof(buffer),
        &bytesReturned
    );

    lastWriteResponse = buffer;
}


// =========================================================

void Galil::AnalogInputRange(uint8_t channel, uint8_t range)
{
    std::string command =
        "AQ " + std::to_string(channel) + "," +
        std::to_string(range) + ";";

    char buffer[1024] = {};
    GSize bytesReturned = 0;

    lastWriteResult = Functions->GCommand(
        g,
        command.c_str(),
        buffer,
        sizeof(buffer),
        &bytesReturned
    );

    lastWriteResponse = buffer;
}


// =========================================================
// ENCODER
// =========================================================

void Galil::WriteEncoder()
{
    std::string command = "WE 0;";

    char buffer[1024] = {};
    GSize bytesReturned = 0;

    lastWriteResult = Functions->GCommand(
        g,
        command.c_str(),
        buffer,
        sizeof(buffer),
        &bytesReturned
    );

    lastWriteResponse = buffer;
}


// =========================================================

int Galil::ReadEncoder()
{
    std::string command = "QE 0;";

    char buffer[1024] = {};
    GSize bytesReturned = 0;

    CheckGReturn(
        Functions->GCommand(
            g,
            command.c_str(),
            buffer,
            sizeof(buffer),
            &bytesReturned
        )
    );

    return std::stoi(buffer);
}


// =========================================================
// CONTROL FUNCTIONS
// =========================================================

void Galil::setSetPoint(int s)
{
    setPoint = s;
}


double Galil::getSetPoint()
{
    return setPoint;
}


void Galil::setKp(double gain)
{
    ControlParameters[0] = gain;
}


double Galil::getKp()
{
    return ControlParameters[0];
}


void Galil::setKi(double gain)
{
    ControlParameters[1] = gain;
}


double Galil::getKi()
{
    return ControlParameters[1];
}


void Galil::setKd(double gain)
{
    ControlParameters[2] = gain;
}


double Galil::getKd()
{
    return ControlParameters[2];
}


// =========================================================
// OUTPUT OPERATOR
// =========================================================

std::ostream& operator<<(std::ostream& output, Galil& galil)
{
    char info[1024] = {};
    char version[1024] = {};

    galil.CheckGReturn(
        galil.Functions->GInfo(
            galil.g,
            info,
            sizeof(info)
        )
    );

    galil.CheckGReturn(
        galil.Functions->GVersion(
            version,
            sizeof(version)
        )
    );

    output << info << "\n\n";
    output << version << "\n\n";

    return output;
}


// =========================================================
// COPY ASSIGNMENT
// =========================================================

Galil& Galil::operator=(const Galil& other)
{
    if (this == &other)
        return *this;

    // Close existing connection
    if (Functions != nullptr && g != 0)
    {
        Functions->GClose(g);
        g = 0;
    }

    // Delete EmbeddedFunctions only if this object owns it
    if (Functions != nullptr && ownsFunctions)
    {
        delete Functions;
    }

    // Copy normal data members
    ControlParameters[0] =
        other.ControlParameters[0];

    ControlParameters[1] =
        other.ControlParameters[1];

    ControlParameters[2] =
        other.ControlParameters[2];

    setPoint = other.setPoint;
    addr = other.addr;

    // Reset write status
    lastWriteResult = G_NO_ERROR;
    lastWriteResponse = "";

    // Create independent EmbeddedFunctions
    Functions =
        new EmbeddedFunctions(USE_SIMULATOR);

    ownsFunctions = true;

    GReturn result =
        Functions->GOpen(addr.c_str(), &g);

    if (result != G_NO_ERROR)
    {
        delete Functions;
        Functions = nullptr;
        g = 0;

        CheckGReturn(result);
    }

    return *this;
}


// =========================================================
// GCLIB ERROR CHECKER
// =========================================================

void Galil::CheckGReturn(GReturn result)
{
    if (result == G_NO_ERROR)
        return;

    std::string errorMessage;

    switch (result)
    {
    case G_GCLIB_ERROR:
        errorMessage =
            "General gclib error";
        break;

    case G_GCLIB_UTILITY_ERROR:
        errorMessage =
            "General gclib utility error";
        break;

    case G_GCLIB_UTILITY_IP_TAKEN:
        errorMessage =
            "Galil IP address is already in use";
        break;

    case G_GCLIB_NON_BLOCKING_READ_EMPTY:
        errorMessage =
            "No Galil data available";
        break;

    case G_TIMEOUT:
        errorMessage =
            "Galil connection timed out";
        break;

    case G_OPEN_ERROR:
        errorMessage =
            "Unable to open Galil connection";
        break;

    case G_INVALID_PREPROCESSOR_OPTIONS:
        errorMessage =
            "Invalid Galil preprocessor options";
        break;

    case G_COMMAND_CALLED_WITH_ILLEGAL_COMMAND:
        errorMessage =
            "Illegal command passed to GCommand";
        break;

    case G_DATA_RECORD_ERROR:
        errorMessage =
            "Galil data record error";
        break;

    case G_UNSUPPORTED_FUNCTION:
        errorMessage =
            "Unsupported Galil function";
        break;

    case G_FIRMWARE_LOAD_NOT_SUPPORTED:
        errorMessage =
            "Galil firmware load not supported";
        break;

    case G_ARRAY_NOT_DIMENSIONED:
        errorMessage =
            "Galil array not dimensioned";
        break;

    case G_ILLEGAL_DATA_IN_PROGRAM:
        errorMessage =
            "Illegal data in Galil program";
        break;

    case G_UNABLE_TO_COMPRESS_PROGRAM_TO_FIT:
        errorMessage =
            "Unable to compress Galil program to fit";
        break;

    case G_BAD_RESPONSE_QUESTION_MARK:
        errorMessage =
            "Galil rejected the command";
        break;

    case G_BAD_VALUE_RANGE:
        errorMessage =
            "Invalid Galil value or connection handle";
        break;

    case G_BAD_FULL_MEMORY:
        errorMessage =
            "Galil library memory error";
        break;

    case G_BAD_LOST_DATA:
        errorMessage =
            "Galil response buffer overflow or data lost";
        break;

    case G_BAD_FILE:
        errorMessage =
            "Galil file error";
        break;

    case G_BAD_ADDRESS:
        errorMessage =
            "Invalid Galil address";
        break;

    default:
        errorMessage =
            "Unknown Galil error: " +
            std::to_string(result);
        break;
    }

    throw std::runtime_error(errorMessage);
}