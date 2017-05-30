#pragma once

#include <vector>
#include <unordered_map>
#include <filesystem>
namespace fs = std::experimental::filesystem;

#include "GenericTypes.hpp"

class Package
{
public:

	/// <summary>
	/// Constructor.
	/// </summary>
	/// <param name="packageObj">The package object.</param>
	/// <param name="packageOrder">[in,out] The package order.</param>
	/// <param name="definedClasses">[in,out] The defined classes.</param>
	Package(const UEObject& packageObj, std::vector<UEObject>& packageOrder, std::unordered_map<UEObject, bool>& definedClasses);

	/// <summary>
	/// Process the classes the package contains.
	/// </summary>
	void Process();

	/// <summary>
	/// Saves the package classes as C++ code.
	/// Files are only generated if there is code present or the generator forces the genertion of empty files.
	/// </summary>
	/// <param name="path">The path to save to.</param>
	/// <returns>true if files got saved, else false.</returns>
	bool Save(const fs::path& path) const;

private:

	/// <summary>
	/// Checks and generates the prerequisites of the script structure.
	/// </summary>
	/// <param name="scriptStructObj">The script structure object.</param>
	void GenerateScriptStructPrerequisites(const UEScriptStruct& scriptStructObj);

	/// <summary>
	/// Checks and generates the prerequisites of the members.
	/// </summary>
	/// <param name="first">The first member in the chain.</param>
	void GenerateMemberPrerequisites(const UEProperty& first);

	/// <summary>
	/// Generates a script structure.
	/// </summary>
	/// <param name="scriptStructObj">The script structure object.</param>
	void GenerateScriptStruct(const UEScriptStruct& scriptStructObj);

	/// <summary>
	/// Generates a constant.
	/// </summary>
	/// <param name="constObj">The constant object.</param>
	void GenerateConst(const UEConst& constObj);

	/// <summary>
	/// Generates an enum.
	/// </summary>
	/// <param name="enumObj">The enum object.</param>
	void GenerateEnum(const UEEnum& enumObj);

	/// <summary>
	/// Checks and generates the prerequisites of the class.
	/// </summary>
	/// <param name="classObj">The class object.</param>
	void GenerateClassPrerequisites(const UEClass& classObj);

	/// <summary>
	/// Generates the class.
	/// </summary>
	/// <param name="classObj">The class object.</param>
	void GenerateClass(const UEClass& classObj);

	enum class FileContentType
	{
		Structs,
		Classes,
		Functions,
		FunctionParameters
	};

	/// <summary>
	/// Generates a file name composed by the game name and the package object.
	/// </summary>
	/// <param name="type">The type of the file.</param>
	/// <returns>
	/// The generated file name.
	/// </returns>
	std::string Package::GenerateFileName(FileContentType type) const;

	/// <summary>
	/// Writes all structs into the appropriate file.
	/// </summary>
	/// <param name="path">The path to save to.</param>
	void SaveStructs(const fs::path& path) const;

	/// <summary>
	/// Writes all classes into the appropriate file.
	/// </summary>
	/// <param name="path">The path to save to.</param>
	void SaveClasses(const fs::path& path) const;

	/// <summary>
	/// Writes all functions into the appropriate file.
	/// </summary>
	/// <param name="path">The path to save to.</param>
	void SaveFunctions(const fs::path& path) const;

	/// <summary>
	/// Writes all function parameters into the appropriate file.
	/// </summary>
	/// <param name="path">The path to save to.</param>
	void SaveFunctionParameters(const fs::path& path) const;

	const UEObject& packageObj;
	std::vector<UEObject>& packageOrder;
	std::unordered_map<UEObject, bool>& definedClasses;

	/// <summary>
	/// Prints the c++ code of the constant.
	/// </summary>
	/// <param name="os">[in] The stream to print to.</param>
	/// <param name="c">The constant to print.</param>
	void PrintConstant(std::ostream& os, const std::pair<std::string, std::string>& c) const;

	std::unordered_map<std::string, std::string> constants;

	struct Enum
	{
		std::string Name;
		std::string FullName;
		std::vector<std::string> Values;
	};

	/// <summary>
	/// Prints the c++ code of the enum.
	/// </summary>
	/// <param name="os">[in] The stream to print to.</param>
	/// <param name="e">The enum to print.</param>
	void PrintEnum(std::ostream& os, const Enum& e) const;

	std::vector<Enum> enums;

	struct Member
	{
		std::string Name;
		std::string Type;

		size_t Offset;
		size_t Size;

		size_t Flags;
		std::string FlagsString;

		std::string Comment;

		/// <summary>
		/// Generates a padding member.
		/// </summary>
		/// <param name="id">The unique name identifier.</param>
		/// <param name="offset">The offset.</param>
		/// <param name="size">The size.</param>
		/// <param name="reason">The reason.</param>
		/// <returns>A padding member.</returns>
		static Member Unknown(size_t id, size_t offset, size_t size, std::string reason);
	};

	/// <summary>
	/// Generates the members of a struct or class.
	/// </summary>
	/// <param name="structObj">The structure object.</param>
	/// <param name="offset">The start offset.</param>
	/// <param name="properties">The properties describing the members.</param>
	/// <param name="members">[out] The members of the struct or class.</param>
	void GenerateMembers(const UEStruct& structObj, size_t offset, const std::vector<UEProperty>& properties, std::vector<Member>& members);

	struct ScriptStruct
	{
		std::string Name;
		std::string FullName;
		std::string NameCpp;
		std::string NameCppFull;

		size_t Size;
		size_t InheritedSize;

		std::vector<Member> Members;

		std::vector<IGenerator::PredefinedMethod> PredefinedMethods;
	};

	/// <summary>
	/// Print the C++ code of the structure.
	/// </summary>
	/// <param name="os">[in] The stream to print to.</param>
	/// <param name="ss">The structure to print.</param>
	void PrintStruct(std::ostream& os, const ScriptStruct& ss) const;

	std::vector<ScriptStruct> scriptStructs;

	struct Method
	{
		struct Parameter
		{
			enum class Type
			{
				Default,
				Out,
				Return
			};

			Type ParamType;
			bool PassByReference;
			std::string CppType;
			std::string Name;
			std::string FlagsString;

			/// <summary>
			/// Generates a valid type of the property flags.
			/// </summary>
			/// <param name="flags">The property flags.</param>
			/// <param name="type">[out] The parameter type.</param>
			/// <returns>true if it is a valid type, else false.</returns>
			static bool MakeType(UEPropertyFlags flags, Type& type);
		};

		size_t Index;
		std::string Name;
		std::string FullName;
		std::vector<Parameter> Parameters;
		std::string FlagsString;
		bool IsNative;
		bool IsStatic;
	};

	/// <summary>
	/// Generates the methods of a class.
	/// </summary>
	/// <param name="classObj">The class object.</param>
	/// <param name="methods">[out] The methods of the class.</param>
	void GenerateMethods(const UEClass& classObj, std::vector<Method>& methods) const;

	/// <summary>
	/// Builds the C++ method signature.
	/// </summary>
	/// <param name="m">The Method to process.</param>
	/// <param name="className">Name of the class.</param>
	/// <param name="inHeader">true if the signature is used as decleration.</param>
	/// <returns>The method signature.</returns>
	std::string BuildMethodSignature(const Method& m, const std::string& className, bool inHeader) const;

	/// <summary>
	/// Builds the c++ method body.
	/// </summary>
	/// <param name="m">The Method to process.</param>
	/// <returns>The method body.</returns>
	std::string BuildMethodBody(const Method& m) const;

	struct Class : public ScriptStruct
	{
		std::vector<std::string> VirtualFunctions;
		std::vector<Method> Methods;
	};

	/// <summary>
	/// Print the C++ code of the class.
	/// </summary>
	/// <param name="os">[in] The stream to print to.</param>
	/// <param name="c">The class to print.</param>
	void PrintClass(std::ostream& os, const Class& c) const;

	std::vector<Class> classes;
};
