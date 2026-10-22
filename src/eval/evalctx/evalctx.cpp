#include "evalctx.hpp"

#include <algorithm>

#include "../evalException.hpp"


std::optional<BigInt> EvalCtx::findVar(const std::string& name) {
	for (auto& var : vars) {
		if (var.first == name) {
			return var.second;
		}
	}

	if (parentCtx) {
		return parentCtx->findVar(name);
	}

	return std::nullopt;
}

//get var value by name
BigInt EvalCtx::getVar(const std::string& name) {
	auto biVar = findVar(name);
	if (biVar.has_value())
		return biVar.value();


	std::string errStr = name + " is undefined";
	std::string noteStr = "Define " + name;
	throw EvalException{ errStr,noteStr };
	return {0};
}

//check if var exists by name
bool EvalCtx::varExists(std::string& name) {
	bool isLocal = std::ranges::any_of(vars,
		[name](const std::pair<std::string,BigInt>& p){return p.first == name;});

	if (isLocal) return isLocal;
	if (parentCtx) return parentCtx->varExists(name);

	return false;
}

//assign var by name and value
//nested example: {a = 5; {a = 3; return 1;}; return a;}    returns 3
void EvalCtx::assignVar(std::string& name, BigInt& bi) {
	for (auto& var : vars) {
		if (var.first == name) {
			var.second = std::move(bi);
			return;
		};
	}

	if (parentCtx) parentCtx->assignVar(name,bi);
}

bool EvalCtx::funcExists(std::string& name) {
	auto isLocal = std::ranges::any_of(userFunctions,[name](UserFunc& func){return func.name == name;});
	if (isLocal) return isLocal;
	if (parentCtx) return parentCtx->funcExists(name);
	return false;
}

//nested example: {f(x) = 2; {f(x) = 3; return 1;};return f(5);}      returns 3, because f(x) is edited
void EvalCtx::assignFunc(UserFunc& userFunc) {
	for (auto& func : userFunctions) {
		if (func.name == userFunc.name) {
			func.params = std::move(userFunc.params);
			func.definition = std::move(userFunc.definition);
			return;
		}
	}
	if (parentCtx) parentCtx->assignFunc(userFunc);
}

void EvalCtx::setParent(EvalCtx& eCtx) {
	parentCtx = &eCtx;
}

bool EvalCtx::resourceExists(uint64_t handle) {
	if (handles.contains(handle)) return true;
	if (parentCtx) {
		return parentCtx->resourceExists(handle);
	}
	return false;
}

Handle* EvalCtx::getResource(uint64_t handle) {
	if (handles.contains(handle)) return handles[handle].get();
	if (parentCtx) {
		return parentCtx->getResource(handle);
	}
	return nullptr;
}

HandleType EvalCtx::getResourceType(uint64_t handle) {
	if (handles.contains(handle)) return handles[handle]->getHandleType();
	if (parentCtx) {
		return parentCtx->getResourceType(handle);
	}
	return HandleType::Null;
}

uint64_t EvalCtx::createFileResource(const std::filesystem::path& path, std::string_view mode) {
	curHandleIndex++;
	auto fHandle = std::make_unique<FileHandle>(path,mode);
	handles.insert({curHandleIndex,std::move(fHandle)});
	return curHandleIndex;
}

bool EvalCtx::closeFileResource(uint64_t handle) {
	if (resourceExists(handle)) {
		handles.erase(handle); //TODO: make this go to parent ctx too
		return true;
	}
	else return false;
}

std::string EvalCtx::getFileResourceNextLine(uint64_t handle) {
	if (resourceExists(handle)) {
		if (getResourceType(handle) != HandleType::File) {
			throw EvalException("Incorrect handle type","This handle is not a handle to a file");
			return "";
		}
		auto fileHandle = dynamic_cast<FileHandle*>(getResource(handle));
		return fileHandle->getLine();
	}
	throw EvalException("No such resource with this handle","This handle given does not exist, use fopen(...) first");
	return "";
}

bool EvalCtx::hasFileResourceNextLine(uint64_t handle) {
	if (resourceExists(handle)) {
		if (getResourceType(handle) != HandleType::File) {
			throw EvalException("Incorrect handle type","This handle is not a handle to a file");
			return false;
		}
		auto fileHandle = dynamic_cast<FileHandle*>(getResource(handle));
		return fileHandle->hasNextLine();
	}
	throw EvalException("No such resource with this handle","This handle given does not exist, use fopen(...) first");
	return false;
}
