typedef struct ilo_info {
	const char* sha256;
	char ilo;
	const char* version;
} ilo_info;

ilo_info* FindILO(const char* sha256);
