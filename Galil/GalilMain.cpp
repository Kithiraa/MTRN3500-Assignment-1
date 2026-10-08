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

        std::cout << "=== CONNECTED ===\n\n";


        // =====================================================
        // 1. LED / DIGITAL OUTPUT TEST
        // =====================================================

        std::cout << "=== LED TEST ===\n";

        // All off
        galil.DigitalOutput(0x0000);
        std::cout << "All LEDs OFF\n";
        std::this_thread::sleep_for(
            std::chrono::milliseconds(500)
        );

        // Turn each lower-bank LED on one at a time
        for (int bit = 0; bit < 8; bit++)
        {
            uint16_t value = static_cast<uint16_t>(1u << bit);

            galil.DigitalOutput(value);

            std::cout
                << "LED " << bit
                << " ON, write success = "
                << galil.CheckSuccessfulWrite()
                << "\n";

            std::this_thread::sleep_for(
                std::chrono::milliseconds(300)
            );
        }

        // All lower LEDs on
        galil.DigitalOutput(0x00FF);

        std::cout
            << "Lower bank all ON, write success = "
            << galil.CheckSuccessfulWrite()
            << "\n";

        std::this_thread::sleep_for(
            std::chrono::milliseconds(700)
        );

        // Alternating pattern
        galil.DigitalOutput(0x00AA);

        std::cout << "Pattern 10101010\n";

        std::this_thread::sleep_for(
            std::chrono::milliseconds(700)
        );

        galil.DigitalOutput(0x0055);

        std::cout << "Pattern 01010101\n";

        std::this_thread::sleep_for(
            std::chrono::milliseconds(700)
        );


        // =====================================================
        // 2. DIGITAL INPUT TEST
        // =====================================================

        std::cout << "\n=== DIGITAL INPUT TEST ===\n";

        galil.DigitalOutput(0x00AA);

        std::this_thread::sleep_for(
            std::chrono::milliseconds(200)
        );

        uint16_t digitalValue =
            galil.DigitalInput();

        std::cout
            << "Digital input value = "
            << digitalValue
            << "\n";

        std::cout << "Individual DI states:\n";

        for (int bit = 0; bit < 8; bit++)
        {
            std::cout
                << "DI" << bit
                << " = "
                << galil.DigitalBitInput(bit)
                << "\n";
        }


        // =====================================================
        // 3. DIGITAL BIT OUTPUT TEST
        // =====================================================

        std::cout << "\n=== DIGITAL BIT OUTPUT TEST ===\n";

        galil.DigitalOutput(0);

        for (int bit = 0; bit < 8; bit++)
        {
            galil.DigitalBitOutput(true, bit);

            std::cout
                << "Set bit " << bit
                << "\n";

            std::this_thread::sleep_for(
                std::chrono::milliseconds(250)
            );
        }

        for (int bit = 0; bit < 8; bit++)
        {
            galil.DigitalBitOutput(false, bit);

            std::cout
                << "Cleared bit " << bit
                << "\n";

            std::this_thread::sleep_for(
                std::chrono::milliseconds(250)
            );
        }


        // =====================================================
        // 4. ANALOG INPUT TEST
        // =====================================================

        std::cout << "\n=== ANALOG INPUT TEST ===\n";

        float analog =
            galil.AnalogInput(0);

        std::cout
            << "Analog input 0 = "
            << analog
            << " V\n";


        // =====================================================
        // 5. MOTOR / ENCODER TEST
        // =====================================================

        std::cout << "\n=== MOTOR TEST ===\n";

        double motorVoltages[] = {
            1.0,
            2.5,
            5.0,
            -2.5
        };

        for (double voltage : motorVoltages)
        {
            std::cout
                << "\nDriving motor at "
                << voltage
                << " V\n";

            // Reset encoder
            galil.WriteEncoder();

            // Drive motor
            galil.AnalogOutput(
                0,
                voltage
            );

            std::cout
                << "Motor write success = "
                << galil.CheckSuccessfulWrite()
                << "\n";

            // Allow motor to reach speed
            std::this_thread::sleep_for(
                std::chrono::milliseconds(500)
            );

            int encoderStart =
                galil.ReadEncoder();

            // Measure for exactly 1 second
            std::this_thread::sleep_for(
                std::chrono::seconds(1)
            );

            int encoderEnd =
                galil.ReadEncoder();

            int countsPerSecond =
                encoderEnd - encoderStart;

            std::cout
                << "Encoder start = "
                << encoderStart
                << "\n";

            std::cout
                << "Encoder end = "
                << encoderEnd
                << "\n";

            std::cout
                << "Speed = "
                << countsPerSecond
                << " counts/sec\n";

            // Stop motor
            galil.AnalogOutput(
                0,
                0.0
            );

            std::cout << "Motor stopped\n";

            std::this_thread::sleep_for(
                std::chrono::milliseconds(500)
            );
        }


        // =====================================================
        // 6. VERIFY MOTOR STOP
        // =====================================================

        std::cout << "\n=== MOTOR STOP CHECK ===\n";

        galil.AnalogOutput(
            0,
            0.0
        );

        std::this_thread::sleep_for(
            std::chrono::milliseconds(500)
        );

        int stopStart =
            galil.ReadEncoder();

        std::this_thread::sleep_for(
            std::chrono::seconds(1)
        );

        int stopEnd =
            galil.ReadEncoder();

        std::cout
            << "Encoder movement while stopped = "
            << stopEnd - stopStart
            << " counts\n";


        // =====================================================
        // 7. PID / SETPOINT TEST
        // =====================================================

        std::cout << "\n=== PID / SETPOINT TEST ===\n";

        galil.setSetPoint(1000);
        galil.setKp(2.5);
        galil.setKi(0.5);
        galil.setKd(0.1);

        std::cout
            << "Setpoint = "
            << galil.getSetPoint()
            << "\n";

        std::cout
            << "Kp = "
            << galil.getKp()
            << "\n";

        std::cout
            << "Ki = "
            << galil.getKi()
            << "\n";

        std::cout
            << "Kd = "
            << galil.getKd()
            << "\n";


        // =====================================================
        // 8. GALIL INFO / VERSION
        // =====================================================

        std::cout << "\n=== GALIL INFO ===\n";

        std::cout << galil;


        // =====================================================
        // 9. FINAL SAFE STATE
        // =====================================================

        std::cout << "\n=== CLEANUP ===\n";

        // Motor off
        galil.AnalogOutput(
            0,
            0.0
        );

        // LEDs off
        galil.DigitalOutput(
            0x0000
        );

        std::cout
            << "Motor OFF\n"
            << "All LEDs OFF\n";

        std::cout << "\n=== TEST COMPLETE ===\n";
    }
    catch (const std::exception& e)
    {
        std::cerr
            << "\nERROR: "
            << e.what()
            << "\n";
    }

    return 0;
}

//#include "Galil.h"
//#include <iostream>
//#include <thread>
//#include <chrono>
//
//int main(void)
//{
//    try
//    {
//        EmbeddedFunctions funcs(true);
//
//        Galil galil(
//            &funcs,
//            "192.168.0.120 -d"
//        );
//
//        std::cout << "=== MOTOR TEST ===\n";
//
//        // Reset encoder
//        galil.WriteEncoder();
//
//        // Drive motor with 2.5 V
//        galil.AnalogOutput(0, 2.5);
//
//        std::cout << "Motor set to 2.5 V\n";
//
//        // Give motor time to start
//        std::this_thread::sleep_for(
//            std::chrono::milliseconds(500)
//        );
//
//        int encoder1 = galil.ReadEncoder();
//
//        std::this_thread::sleep_for(
//            std::chrono::seconds(1)
//        );
//
//        int encoder2 = galil.ReadEncoder();
//
//        // Stop motor
//        galil.AnalogOutput(0, 0.0);
//
//        int countsPerSecond =
//            encoder2 - encoder1;
//
//        std::cout << "Encoder start: "
//            << encoder1 << "\n";
//
//        std::cout << "Encoder end: "
//            << encoder2 << "\n";
//
//        std::cout << "Speed: "
//            << countsPerSecond
//            << " counts/sec\n";
//
//        std::cout << "Motor stopped\n";
//    }
//    catch (const std::exception& e)
//    {
//        std::cerr << "ERROR: "
//            << e.what()
//            << "\n";
//    }
//
//    return 0;
//}



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