#pragma once
class MultiplayerHelper
{
public:
    MultiplayerHelper();
    ~MultiplayerHelper();
    enum class packetType { invalid, allPlayersData, singlePlayerData, globalData, command }; 

    static sf::Packet createPacketRaw(packetType type);
    static void appendToPacket(sf::Packet & packet, unsigned char* bytes, size_t count);

    template<typename T, typename A>
    static void appendToPacket(sf::Packet & packet, std::vector<T, A> const& bytes) {
        appendToPacket(packet, const_cast<unsigned char*>(bytes.data()), bytes.size() * sizeof(T));
    }
    template<typename T>
    static void appendToPacket(sf::Packet & packet, T bytes) {
        appendToPacket(packet, reinterpret_cast<unsigned char*> (&bytes), sizeof(T));
    }


};
