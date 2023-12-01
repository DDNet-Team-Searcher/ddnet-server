class DDTS
{
public:
    int fd;
	void *sharedMemory;
    unsigned int happeningId;

	DDTS(unsigned int happeningId);
    ~DDTS();
	bool CheckShutdownSignal();
	void Shutdown();
};
