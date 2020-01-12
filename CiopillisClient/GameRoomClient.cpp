//#include "GameRoomClient.h"
//#include "External.h"
//#include <iostream>
//
//void GameRoomClientSystem::RunMainThread()
//{
//	std::cout << "Running Main Thread" << std::endl;
//	while (true) {
//
//		if (signal_access(READ_TYPE, false))
//			break;
//
//		char buff[4096];
//
//		std::vector<Packet> new_packets;
//		Packet pack;
//		memcpy(buff, "test", strlen("test"));
//		buff[strlen("test")] = '\0';
//		pack.insert(pack.begin(), buff, buff + strlen(buff) + 1);
//		new_packets.push_back(pack);
//
//		send_packets(new_packets);
//
//		SleepFunc(10);
//
//		auto p = gather_packets();
//
//		if (signal_access(READ_TYPE, false))
//			break;
//
//		if (p.empty())
//			break;
//
//		memset(buff, '\0', 4096);
//		strcpy(buff + strlen(buff), &p[0][0]);
//		queue_access(WRITE_TYPE, buff);
//
//		std::cout << "Sent" << "\n";
//	}
//}
//
//void GameRoomClientSystem::Execute()
//{
//	
//}
