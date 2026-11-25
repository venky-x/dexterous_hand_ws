
#include "scs0009_driver/SCS0009Driver.h"

#include <algorithm>
#include <set>

SCS0009Driver::SCS0009Driver()
    : connected_(false)
{
}

// --------------------------------------------------
// Connection management
// --------------------------------------------------

bool SCS0009Driver::connect(
    const std::string& port,
    int baudrate)
{
    if (connected_)
    {
        disconnect();
    }

    connected_ = servo_.begin(baudrate, port.c_str());

    return connected_;
}

void SCS0009Driver::disconnect()
{
    if (connected_)
    {
        servo_.end();
        connected_ = false;
    }
}

bool SCS0009Driver::isConnected() const
{
    return connected_;
}

// --------------------------------------------------
// Servo communication
// --------------------------------------------------

bool SCS0009Driver::isValidServoId(int servo_id) const
{
    return servo_id >= 0 && servo_id < 0xFE;
}

bool SCS0009Driver::ping(int servo_id)
{
    if (!connected_ || !isValidServoId(servo_id))
    {
        return false;
    }

    return servo_.Ping(servo_id) == servo_id;
}

// --------------------------------------------------
// Torque control
// --------------------------------------------------

bool SCS0009Driver::enableTorque(int servo_id)
{
    if (!connected_ || !isValidServoId(servo_id))
    {
        return false;
    }

    return servo_.EnableTorque(servo_id, 1) == 1;
}

bool SCS0009Driver::disableTorque(int servo_id)
{
    if (!connected_ || !isValidServoId(servo_id))
    {
        return false;
    }

    return servo_.EnableTorque(servo_id, 0) == 1;
}

// --------------------------------------------------
// Individual position control
// --------------------------------------------------

bool SCS0009Driver::setPosition(
    int servo_id,
    int position,
    int time,
    int speed)
{
    if (!connected_ || !isValidServoId(servo_id))
    {
        return false;
    }

    if (position < 0 || position > 65535 ||
        time < 0 || time > 65535 ||
        speed < 0 || speed > 65535)
    {
        return false;
    }

    return servo_.WritePos(
        servo_id,
        static_cast<u16>(position),
        static_cast<u16>(time),
        static_cast<u16>(speed)
    ) == 1;
}

// --------------------------------------------------
// Registered position control
// --------------------------------------------------

bool SCS0009Driver::registerPosition(
    int servo_id,
    int position,
    int time,
    int speed)
{
    if (!connected_ || !isValidServoId(servo_id))
    {
        return false;
    }

    if (position < 0 || position > 65535 ||
        time < 0 || time > 65535 ||
        speed < 0 || speed > 65535)
    {
        return false;
    }

    return servo_.RegWritePos(
        servo_id,
        static_cast<u16>(position),
        static_cast<u16>(time),
        static_cast<u16>(speed)
    ) == 1;
}

bool SCS0009Driver::executeRegisteredCommands()
{
    if (!connected_)
    {
        return false;
    }

    // Broadcast action command: no individual servo acknowledgement.
    return servo_.RegWriteAction() == 1;
}

// --------------------------------------------------
// Synchronous multi-servo position control
// --------------------------------------------------

bool SCS0009Driver::syncWritePosition(
    const std::vector<int>& servo_ids,
    const std::vector<int>& positions,
    const std::vector<int>& times,
    const std::vector<int>& speeds)
{
    if (!connected_)
    {
        return false;
    }

    const size_t count = servo_ids.size();

    if (count == 0 || count > 35)
    {
        return false;
    }

    if (positions.size() != count)
    {
        return false;
    }

    if (!times.empty() && times.size() != count)
    {
        return false;
    }

    if (!speeds.empty() && speeds.size() != count)
    {
        return false;
    }

    std::set<int> unique_ids;

    for (size_t i = 0; i < count; ++i)
    {
        if (!isValidServoId(servo_ids[i]) ||
            positions[i] < 0 || positions[i] > 65535 ||
            (!times.empty() &&
             (times[i] < 0 || times[i] > 65535)) ||
            (!speeds.empty() &&
             (speeds[i] < 0 || speeds[i] > 65535)))
        {
            return false;
        }

        if (!unique_ids.insert(servo_ids[i]).second)
        {
            return false;
        }
    }

    std::vector<u8> ids(count);
    std::vector<u16> position_values(count);
    std::vector<u16> time_values;
    std::vector<u16> speed_values;

    for (size_t i = 0; i < count; ++i)
    {
        ids[i] = static_cast<u8>(servo_ids[i]);

        position_values[i] =
            static_cast<u16>(positions[i]);
    }

    if (!times.empty())
    {
        time_values.resize(count);

        for (size_t i = 0; i < count; ++i)
        {
            time_values[i] = static_cast<u16>(times[i]);
        }
    }

    if (!speeds.empty())
    {
        speed_values.resize(count);

        for (size_t i = 0; i < count; ++i)
        {
            speed_values[i] = static_cast<u16>(speeds[i]);
        }
    }

    servo_.SyncWritePos(
        ids.data(),
        static_cast<u8>(count),
        position_values.data(),
        times.empty() ? nullptr : time_values.data(),
        speeds.empty() ? nullptr : speed_values.data()
    );

    return true;
}

// --------------------------------------------------
// Feedback
// --------------------------------------------------

bool SCS0009Driver::readFeedback(
    int servo_id,
    Feedback& feedback)
{
    if (!connected_ || !isValidServoId(servo_id))
    {
        return false;
    }

    // Read the complete feedback register block.
    // SCS0009 feedback block: addresses 56 through 70 = 15 bytes.
    const int bytes_read = servo_.FeedBack(servo_id);

    if (bytes_read != 15 || servo_.getLastError() != 0)
    {
        return false;
    }

    // Read the values from the SDK's cached feedback buffer.
    Feedback result;

    result.position = servo_.ReadPos(-1);
    result.speed = servo_.ReadSpeed(-1);
    result.load = servo_.ReadLoad(-1);
    result.voltage = servo_.ReadVoltage(-1);
    result.temperature = servo_.ReadTemper(-1);
    result.moving = servo_.ReadMove(-1);
    result.current = servo_.ReadCurrent(-1);

    // Only publish the completed snapshot.
    feedback = result;

    return true;
}
