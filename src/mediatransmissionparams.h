#ifndef MEDIATRANSMISSIONPARAMS_H
#define MEDIATRANSMISSIONPARAMS_H

enum class FrameType
{
    IFRAME = 0,
    PFRAME = 1,
    BFRAME = 2,
    AUDIOFRAME = 3,
    TRANSPARENT = 4
};

enum class PackageType
{
    ATOMIC = 0,
    FIRST,
    LAST,
    INTERMEDIATE
};

struct MediaTransmissionParams{
     uint16_t packageSerialNumber;
     uint8_t logicalChannelNumber;
     FrameType frameType;
     PackageType packageType;

};

#endif // MEDIATRANSMISSIONPARAMS_H
