#pragma once
#include "System.h"
#include <thread>
#include <memory>

class NetworkSystem : public ISystem
{
public:
	NetworkSystem() : ISystem("NetworkSystem") {}

	std::shared_ptr<std::thread> nt = nullptr;

	static void ThreadRun(NetworkSystem* parentSystem)
	{
		
	}

	void Initialize() override
	{
		nt = std::make_shared<std::thread>(ThreadRun, this);
	}

	void Execute() override {}

};