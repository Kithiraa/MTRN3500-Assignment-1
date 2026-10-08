#include "EmbeddedFunctions.h"

using namespace System;
using namespace System::Threading;

int main(void)
{
    Console::WriteLine("PART B TEST START");

    EmbeddedFunctions^ funcs = gcnew EmbeddedFunctions();

    try
    {
        Console::WriteLine("Connecting...");
        funcs->GOpen("127.0.0.1", 26000);
        Console::WriteLine("Connected");

        int value = 0;

        for (int bit = 0; bit < 8; bit++)
        {
            value |= (1 << bit);

            String^ command =
                "OP " + value.ToString() + ",0";

            Console::WriteLine("Sending: " + command);

            String^ response = funcs->GCommand(command);

            Console::WriteLine("Response: [" + response + "]");

            Thread::Sleep(500);
        }

        funcs->GClose();
        Console::WriteLine("Finished");
    }
    catch (Exception^ e)
    {
        Console::WriteLine("ERROR: " + e->Message);
    }

    Console::ReadLine();
    return 0;
}