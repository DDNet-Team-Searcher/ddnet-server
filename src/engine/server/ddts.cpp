// WARNING: if you know how to code, please dont check this code
// you will have a heart attack

#include <base/system.h>
#include <arpa/inet.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <string>
#include <sys/mman.h>
#include <unistd.h>

#include "ddts.h"
#include <protos/common.pb.h>
#include <protos/request.pb.h>

const std::string HOST = "127.0.0.1";
const std::string SHM_NAME = "/ddts";
const uint16_t PORT = 42069;
const size_t SHM_SIZE = 10;

CDDTS::CDDTS(unsigned int Id, unsigned int HappeningId) :
	m_HappeningId(HappeningId), m_Id(Id), m_Shutdown(false)
{
	fd = shm_open(SHM_NAME.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IXUSR);

    dbg_assert(fd != -1, "Failed to get shared memory fd");
    dbg_assert(!ftruncate(fd, SHM_SIZE), "Failed to resize shared memory fd");

	sharedMemory = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if(sharedMemory == MAP_FAILED)
	{
		dbg_msg("ddts", "mmap error");
		// not gonna lie, we are fucked
	}
}

bool CDDTS::CheckShutdownSignal()
{
	uint8_t *mem = static_cast<uint8_t *>(sharedMemory);

	if(mem[m_Id] == 1)
	{
		m_Shutdown = true;
		return true;
	}

	return false;
}

void CDDTS::Shutdown() const
{
	if(!m_Shutdown)
	{
		sockaddr_in serv_addr;
		int sock = socket(AF_INET, SOCK_STREAM, 0);

		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(PORT);

		inet_pton(AF_INET, HOST.data(), &serv_addr.sin_addr);

		(void)connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

		Request request;
		request.set_action(Request_Action::Request_Action_SHUTDOWN);
		request.set_id(m_HappeningId);
		request.set_origin(::Origin::DDNET);

		const size_t size = request.ByteSizeLong();
		uint8_t *bytes = new uint8_t[size];

		request.SerializeToArray(bytes, size);

		send(sock, bytes, size, 0);

		std::vector<uint8_t> gottem(1024);

		// wait for the response owo. works for now
		recv(sock, gottem.data(), gottem.size(), 0);
	}

	uint8_t *mem = static_cast<uint8_t *>(sharedMemory);
	mem[m_Id] = 0;

	munmap(sharedMemory, SHM_SIZE);
	close(fd);
}
