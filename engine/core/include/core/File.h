#pragma once

namespace INF
{
	class File
	{
	public:
		File();

		bool Open(std::string_view filename);
		const std::vector<char>& Contents() const { return m_contents; }
	private:
		std::vector<char> m_contents;
	};
}