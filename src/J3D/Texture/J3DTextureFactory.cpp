#include "J3D/Texture/J3DTextureFactory.hpp"
#include "J3D/Texture/J3DTextureLoader.hpp"
#include "J3D/Data/J3DBlock.hpp"

#include "glad/glad.h"
#include "bstream.h"

#ifdef _DEBUG
#include <stb_image.h>
#include <stb_image_write.h>
#include <filesystem>
#endif

#include <cmath>

const float ONE_EIGHTH = 0.125f;
const float ONE_HUNDREDTH = 0.01f;

J3DTextureFactory::J3DTextureFactory(J3DTextureBlock* srcBlock, bStream::CStream* stream) {
  mBlock = srcBlock;

  stream->seek(mBlock->NameTableOffset);
  mNameTable.Deserialize(stream);
}

std::shared_ptr<J3DTexture> J3DTextureFactory::Create(bStream::CStream* stream, uint32_t index) {
  uint32_t dataOffset = mBlock->TexTableOffset + (index * TEXTURE_ENTRY_SIZE);
  stream->seek(dataOffset);

  std::string textureName = mNameTable.GetName(index);

  J3DTextureLoader btiLoader{};
  return btiLoader.Load(textureName, stream);
}

