#include "Galil.h"
#include <iostream>
#include <thread>
#include <chrono>

int main(void)
{
    try
    {
        EmbeddedFunctions funcs(true);

        Galil galil(
            &funcs,
            "192.168.0.120 -d"
        );

        std::cout << "=== MOTOR TEST ===\n";

        // Reset encoder
        galil.WriteEncoder();

        // Drive motor with 2.5 V
        galil.AnalogOutput(0, 2.5);

        std::cout << "Motor set to 2.5 V\n";

        // Give motor time to start
        std::this_thread::sleep_for(
            std::chrono::milliseconds(500)
        );

        int encoder1 = galil.ReadEncoder();

        std::this_thread::sleep_for(
            std::chrono::seconds(1)
        );

        int encoder2 = galil.ReadEncoder();

        // Stop motor
        galil.AnalogOutput(0, 0.0);

        int countsPerSecond =
            encoder2 - encoder1;

        std::cout << "Encoder start: "
            << encoder1 << "\n";

        std::cout << "Encoder end: "
            << encoder2 << "\n";

        std::cout << "Speed: "
            << countsPerSecond
            << " counts/sec\n";

        std::cout << "Motor stopped\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "ERROR: "
            << e.what()
            << "\n";
    }

    return 0;
}



/*#include "Galil.h"
#include <iostream>

int main(void)
{
    try
    {
        EmbeddedFunctions funcs(true);

        Galil galil(
            &funcs,
            "192.168.0.120 -d"
        );

        std::cout << "=== CONNECTED ===\n\n";


        // -------------------------------------------------
        // DIGITAL OUTPUT TESTS
        // -------------------------------------------------

        std::cout << "=== DIGITAL OUTPUTS ===\n";

        galil.DigitalOutput(0);
        std::cout << "DigitalOutput(0): "
            << galil.CheckSuccessfulWrite()
            << "\n";

        galil.DigitalOutput(1);
        std::cout << "DigitalOutput(1): "
            << galil.CheckSuccessfulWrite()
            << "\n";

        galil.DigitalOutput(255);
        std::cout << "DigitalOutput(255): "
            << galil.CheckSuccessfulWrite()
            << "\n";

        galil.DigitalOutput(0xFFFF);
        std::cout << "DigitalOutput(0xFFFF): "
            << galil.CheckSuccessfulWrite()
            << "\n";


        // -------------------------------------------------
        // DIGITAL BYTE OUTPUT
        // -------------------------------------------------

        std::cout << "\n=== DIGITAL BYTE OUTPUT ===\n";

        galil.DigitalOutput(0);

        galil.DigitalByteOutput(0, 0xAA);
        std::cout << "Low bank = 0xAA: "
            << galil.CheckSuccessfulWrite()
            << "\n";

        galil.DigitalByteOutput(1, 0x55);
        std::cout << "High bank = 0x55: "
            << galil.CheckSuccessfulWrite()
            << "\n";


        // -------------------------------------------------
        // DIGITAL BIT OUTPUT
        // -------------------------------------------------

        std::cout << "\n=== DIGITAL BIT OUTPUT ===\n";

        galil.DigitalOutput(0);

        galil.DigitalBitOutput(true, 0);
        std::cout << "Set bit 0: "
            << galil.CheckSuccessfulWrite()
            << "\n";

        galil.DigitalBitOutput(true, 3);
        std::cout << "Set bit 3: "
            << galil.CheckSuccessfulWrite()
            << "\n";

        galil.DigitalBitOutput(false, 0);
        std::cout << "Clear bit 0: "
            << galil.CheckSuccessfulWrite()
            << "\n";


        // -------------------------------------------------
        // DIGITAL INPUTS
        // -------------------------------------------------

        std::cout << "\n=== DIGITAL INPUTS ===\n";

        // Simulator links DO0-7 to DI0-7
        galil.DigitalOutput(0b10101010);

        std::cout << "DigitalInput: "
            << galil.DigitalInput()
            << "\n";

        std::cout << "Low input bank: "
            << static_cast<int>(galil.DigitalByteInput(0))
            << "\n";

        std::cout << "High input bank: "
            << static_cast<int>(galil.DigitalByteInput(1))
            << "\n";

        for (int bit = 0; bit < 8; bit++)
        {
            std::cout << "DI" << bit << ": "
                << galil.DigitalBitInput(bit)
                << "\n";
        }


        // -------------------------------------------------
        // ANALOG INPUT
        // -------------------------------------------------

        std::cout << "\n=== ANALOG INPUT ===\n";

        std::cout << "Analog input 0: "
            << galil.AnalogInput(0)
            << " V\n";


        // -------------------------------------------------
        // ANALOG RANGE
        // -------------------------------------------------

        std::cout << "\n=== ANALOG INPUT RANGE ===\n";

        galil.AnalogInputRange(0, 2);

        std::cout << "AQ channel 0 range 2: "
            << galil.CheckSuccessfulWrite()
            << "\n";


        // -------------------------------------------------
        // ANALOG OUTPUT
        // -------------------------------------------------

        std::cout << "\n=== ANALOG OUTPUT ===\n";

        galil.AnalogOutput(0, 2.50);

        std::cout << "AnalogOutput(0, 2.50): "
            << galil.CheckSuccessfulWrite()
            << "\n";


        // -------------------------------------------------
        // ENCODER
        // -------------------------------------------------

        std::cout << "\n=== ENCODER ===\n";

        galil.WriteEncoder();

        std::cout << "WriteEncoder: "
            << galil.CheckSuccessfulWrite()
            << "\n";

        std::cout << "ReadEncoder: "
            << galil.ReadEncoder()
            << "\n";


        // -------------------------------------------------
        // CONTROL PARAMETERS
        // -------------------------------------------------

        std::cout << "\n=== CONTROL PARAMETERS ===\n";

        galil.setSetPoint(1000);
        galil.setKp(2.5);
        galil.setKi(0.5);
        galil.setKd(0.1);

        std::cout << "Setpoint: "
            << galil.getSetPoint()
            << "\n";

        std::cout << "Kp: "
            << galil.getKp()
            << "\n";

        std::cout << "Ki: "
            << galil.getKi()
            << "\n";

        std::cout << "Kd: "
            << galil.getKd()
            << "\n";


        // -------------------------------------------------
        // OPERATOR <<
        // -------------------------------------------------

        std::cout << "\n=== GINFO / GVERSION ===\n";

        std::cout << galil;


        // -------------------------------------------------
        // COPY CONSTRUCTOR
        // -------------------------------------------------

        std::cout << "\n=== COPY CONSTRUCTOR ===\n";

        Galil copy(galil);

        std::cout << "Copy setpoint: "
            << copy.getSetPoint()
            << "\n";

        std::cout << "Copy Kp: "
            << copy.getKp()
            << "\n";

        std::cout << "Copy Ki: "
            << copy.getKi()
            << "\n";

        std::cout << "Copy Kd: "
            << copy.getKd()
            << "\n";


        // -------------------------------------------------
        // COPY ASSIGNMENT
        // -------------------------------------------------

        std::cout << "\n=== COPY ASSIGNMENT ===\n";

        Galil assigned;

        assigned = galil;

        std::cout << "Assigned setpoint: "
            << assigned.getSetPoint()
            << "\n";

        std::cout << "Assigned Kp: "
            << assigned.getKp()
            << "\n";

        std::cout << "Assigned Ki: "
            << assigned.getKi()
            << "\n";

        std::cout << "Assigned Kd: "
            << assigned.getKd()
            << "\n";


        // -------------------------------------------------
        // CLEAN OUTPUT STATE
        // -------------------------------------------------

        galil.DigitalOutput(0);

        std::cout << "\n=== TEST COMPLETE ===\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "\nERROR: "
            << e.what()
            << "\n";
    }

    return 0;
}*/