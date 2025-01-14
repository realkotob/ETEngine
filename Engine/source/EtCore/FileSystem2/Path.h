#pragma once


namespace et {
namespace core {


//------
// Path
//
// Platform independent path utility
//
class Path
{
	// definitions
	//-------------
	struct Component
	{
		Component(uint16 const start, uint16 const length, std::string const& inPath);

		uint16 m_Start;
		uint16 m_Length;
		HashString m_Hash;
	};

	static std::string const s_InvalidString;
	static std::vector<char> const s_Delimiters;

	// static functionality
	//----------------------
	static bool IsDelim(char const c);
	static bool IsThis(Component const& comp);
	static bool IsParent(Component const& comp);

public:
	enum class E_Type : uint8
	{
		Relative,
		Absolute,
		AbsoluteVolume,

		Invalid
	};

	// construct destruct
	//--------------------
	Path() = default;
	Path(std::string const& inPath);
	Path(Path const& base, std::string const& inPath);
	Path(Path const& base, Path const& other);

	// functionality
	//---------------
	void Append(Path const& other);
	void PopFront();
	void PopBack();
	void MakeRelative(Path const& relativeTo);

	// accessors
	//-----------
	std::string const& Get() const { return m_Impl; }
	HashString GetId() const { return m_Id; }

	std::string GetName() const;

	std::string GetRawName() const;
	HashString GetRawNameId() const;

	std::string GetExtension() const;
	HashString GetExtensionId() const;

	std::string GetParentPath() const;
	bool HasParentPath() const;

	E_Type GetType() const { return m_Type; }
	bool IsFile() const; // or directory

	std::string GetVolume() const;
	HashString GetVolumeId() const;

	// utility
	//---------
	void Swap(Path& other);

private: 
	void ConstructComponentsAndName(size_t const startPos, std::string const& inPath);
	size_t GetParentMoveComponentCount() const;


	// Data
	///////

	std::string m_Impl;
	HashString m_Id;

	std::vector<Component> m_Components;

	E_Type m_Type = E_Type::Invalid;
	bool m_HasExtension = false;
};


} // namespace core
} // namespace et
