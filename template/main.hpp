namespace mm_template
{
	void main();
	void load_patches();
	void timeGetTime_timeFix();

	class main final
	{
	public:
		static std::uint32_t API_PlayerName;
		static std::uint32_t API_GetLevel;
		static std::uint32_t API_GetMapName;
		static std::uint32_t API_GetJobID;
	};

}