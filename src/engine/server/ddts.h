class CDDTS
{
public:
	int fd;
	void *sharedMemory;
	unsigned int m_HappeningId;
	unsigned int m_Id;
	bool m_Shutdown;

	CDDTS(unsigned int Id, unsigned int HappeningId);
	bool CheckShutdownSignal();
	void Shutdown() const;
};
