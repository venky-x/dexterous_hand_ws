#include <iostream>
#include <cstdlib>

#include "SCSCL.h"

int main()
{
    // Communication settings
    const char* SERIAL_PORT = "/dev/ttyACM_servo";
    const int BAUD_RATE = 1000000;
    const int SERVO_ID = 1;

    SCSCL servo;

    std::cout << "Connecting to SCS0009..." << std::endl;
    std::cout << "Port: " << SERIAL_PORT << std::endl;
    std::cout << "Baud rate: " << BAUD_RATE << std::endl;
    std::cout << "Servo ID: " << SERVO_ID << std::endl;

    // Open serial connection
    if (!servo.begin(BAUD_RATE, SERIAL_PORT))
    {
        std::cerr << "ERROR: Could not open serial connection."
                  << std::endl;
        return 1;
    }

    std::cout << "\nSerial connection opened." << std::endl;

    // Ping servo
    int ping_result = servo.Ping(SERVO_ID);

    if (ping_result < 0)
    {
        std::cerr << "ERROR: Servo did not respond to ping."
                  << std::endl;
        std::cerr << "SDK error: "
                  << static_cast<int>(servo.getLastError())
                  << std::endl;

        servo.end();
        return 1;
    }

    std::cout << "Ping successful. Responding servo ID: "
              << ping_result << std::endl;

    // Read feedback directly from the servo
    std::cout << "\n===== SERVO FEEDBACK =====" << std::endl;

    int position = servo.ReadPos(SERVO_ID);
    int speed = servo.ReadSpeed(SERVO_ID);
    int voltage = servo.ReadVoltage(SERVO_ID);
    int temperature = servo.ReadTemper(SERVO_ID);
    int current = servo.ReadCurrent(SERVO_ID);

    std::cout << "Position:    " << position << std::endl;
    std::cout << "Speed:       " << speed << std::endl;
    std::cout << "Voltage:     " << voltage << " (raw)" << std::endl;
    std::cout << "Temperature: " << temperature << " (raw)" << std::endl;
    std::cout << "Current:     " << current << " (raw)" << std::endl;

    // Close serial connection
    servo.end();

    std::cout << "\nSerial connection closed." << std::endl;
    std::cout << "Read test completed." << std::endl;

    return 0;
}
