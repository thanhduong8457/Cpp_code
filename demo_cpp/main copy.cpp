#include <iostream>
#include <vector>

const uint32_t FRAME_CRC_POLY = 0x1F23B;  // Frame CRC polynomial
const uint32_t HEADER_CRC_POLY = 0x0F4D;  // Header CRC polynomial

/// @brief 
/// @param data 
/// @param sizeCrc 
/// @param poly 
/// @return 
uint32_t calculateCRC(uint8_t* data, uint32_t lengthInByte, unsigned int sizeCrc, uint32_t poly) {
    uint32_t maskMSB = 1 << sizeCrc;
    uint32_t mask = maskMSB - 1;
    uint32_t crc = mask;
    std::cout << "maskMSB 0x" << std::hex << maskMSB << std::endl;
    std::cout << "mask 0x" << std::hex << mask << std::endl;

    for (uint8_t index = 0x0; index < lengthInByte; ++index) {
        std::cout << "data byte 0x" << std::hex << (static_cast<uint32_t>(data[index])) << std::endl;
        std::cout << "data byte after shift 0x" << std::hex << (static_cast<uint32_t>(data[index]) << (sizeCrc - 8)) << std::endl;
        crc ^= ((static_cast<uint32_t>(data[index]) << (sizeCrc - 8)));
        for (int i = 0; i < 8; ++i) {
            if (crc & (maskMSB >> 1)) {
                crc = (crc << 1) ^ poly;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc & mask;
}

/// @brief 
/// @param data 
/// @param transmittedCRC 
/// @param isFrameCRC 
/// @return 
bool verifyCRC(uint8_t* data, uint32_t dataLength, uint32_t transmittedCRC, bool isFrameCRC = true) {
    uint32_t calculatedCRC;
    if (isFrameCRC) {
        calculatedCRC = calculateCRC(data, dataLength, 24, FRAME_CRC_POLY);
    } else {
        calculatedCRC = calculateCRC(data, dataLength, 11, HEADER_CRC_POLY);
    }

    return (calculatedCRC == transmittedCRC);
}

int main() {
    // Initialize vectors using constructor with initializer list
    uint8_t* payloadData = NULL; // Example payload
    uint8_t* headerData = NULL;// Example header

    headerData = new uint8_t[2];
    payloadData = new uint8_t[3];

    headerData[0] = 0x12;
    headerData[1] = 0x34;

    payloadData[0] = 0xAB;
    payloadData[1] = 0xCD;
    payloadData[2] = 0xEF;

    uint32_t frameCRC = calculateCRC(payloadData, 3, 24, FRAME_CRC_POLY);
    std::cout << "Calculated Frame CRC (24-bit): 0x" << std::hex << frameCRC << std::endl;

    uint16_t headerCRC = calculateCRC(headerData, 2, 11, HEADER_CRC_POLY);
    std::cout << "Calculated Header CRC (11-bit): 0x" << std::hex << headerCRC << std::endl;

    // uint32_t transmittedFrameCRC = frameCRC;

    // if (verifyCRC(payloadData, 3, transmittedFrameCRC)) {
    //     std::cout << "Frame CRC verification passed!" << std::endl;
    // } else {
    //     std::cout << "Frame CRC verification failed!" << std::endl;
    // }

    // uint32_t transmittedHeaderCRC = headerCRC;

    // if (verifyCRC(headerData, 2, transmittedHeaderCRC, false)) {
    //     std::cout << "Header CRC verification passed!" << std::endl;
    // } else {
    //     std::cout << "Header CRC verification failed!" << std::endl;
    // }

    return 0;
}
