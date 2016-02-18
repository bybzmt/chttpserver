
//fnv1a hash算法
uint32_t fnv1a_hash(char data[], size_t len)
{
	uint32_t p = 16777619, hash = 2166136261L;
	size_t i = 0;

	for (i=0; i<len; i++) {
		hash = (hash ^ data[i]) * p;
	}

	return hash;
}
