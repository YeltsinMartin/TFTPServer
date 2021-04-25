#pragma once
#include"ServerBase.h"

class Server final : public ServerBase {

public:
	Server();

	virtual void stepNetwork();

	~Server();
};