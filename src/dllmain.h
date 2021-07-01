enum variables {
	m_nGameWidth = 1366
};

unsigned int JenkinsOneAtATimeCompute(const char* key, size_t len) {
	unsigned int hash, i;
	for (hash = i = 0; i < len; ++i) {
		hash += key[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}