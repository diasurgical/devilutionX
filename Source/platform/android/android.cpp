#include "engine/assets.hpp"
#include "init.h"
#include "mpq/mpq_reader.hpp"

#include <jni.h>

namespace devilution {
namespace {

bool AreFontsOutOfDate(const char *mpqPath)
{
	int32_t error = 0;
	std::optional<MpqArchive> archive = MpqArchive::Open(mpqPath, error);
	if (error != 0 || !archive)
		return false;

	const char filename[] = "fonts\\VERSION";
	const MpqArchive::FileHash fileHash = MpqArchive::CalculateFileHash(filename);
	uint32_t fileNumber;
	if (!archive->GetFileNumber(fileHash, fileNumber))
		return true;

	AssetRef ref;
	ref.archive = &*archive;
	ref.fileNumber = fileNumber;
	ref.filename = filename;

	const size_t size = ref.size();
	AssetHandle handle = OpenAsset(std::move(ref), false);
	if (!handle.ok())
		return true;

	std::unique_ptr<char[]> version_contents { new char[size] };
	handle.read(version_contents.get(), size);
	return string_view { version_contents.get(), size } != font_mpq_version;
}

} // namespace
} // namespace devilution

extern "C" {
JNIEXPORT jboolean JNICALL Java_org_diasurgical_devilutionx_DevilutionXSDLActivity_areFontsOutOfDate(JNIEnv *env, jclass cls, jstring fonts_mpq)
{
	const char *mpqPath = env->GetStringUTFChars(fonts_mpq, nullptr);
	bool outOfDate = devilution::AreFontsOutOfDate(mpqPath);
	env->ReleaseStringUTFChars(fonts_mpq, mpqPath);
	return outOfDate;
}
}
