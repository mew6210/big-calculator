#pragma once
#include <optional>
#include <vector>
#include "../../bigint/bigint.hpp"
#include <string>
#include <unordered_map>

#include "../../handle/handle.hpp"

struct UserFunc;

/*
	@brief struct for keeping information current evaluation "runtime"
*/
struct EvalCtx {
	using handlePtr = std::unique_ptr<Handle>;
	EvalCtx* parentCtx = nullptr;

	std::vector<std::pair<std::string, BigInt>> vars;
	std::string fileToExec;
	bool shouldPrint = true;
	std::vector<UserFunc> userFunctions;
	bool showLexerOutput = false;
	bool showParserOutput = false;
	std::unordered_map<uint64_t,handlePtr> handles;
	int curHandleIndex = 0;

	BigInt getVar(const std::string& name);
	std::optional<BigInt> findVar(const std::string& name);
	bool varExists(std::string& name);
	void assignVar(std::string& name, BigInt& bi);

	bool funcExists(std::string& name);
	void assignFunc(UserFunc& func);
	void setParent(EvalCtx&);

	uint64_t createFileResource(const std::filesystem::path& path, std::string_view mode);
	bool closeFileResource(uint64_t handle);

};

#include "../userfunc/userfunc.hpp"