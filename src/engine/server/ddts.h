class CDDTS
{
public:
    int fd;
	void *sharedMemory;
    unsigned int m_HappeningId;
    unsigned int m_Id;
    bool m_Shutdown;

	CDDTS(unsigned int id, unsigned int happeningId);
	bool CheckShutdownSignal();
	void Shutdown();
};
