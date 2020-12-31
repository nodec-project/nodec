#include "Graphics/Texture.hpp"
#include "Utils.hpp"

#include <nodec/unicode.hpp>

#include <DirectXTex.h>

Texture::Texture(Graphics* graphics, const std::string& path)
{

    enum class ImageType
    {
        TGA,
        WIC
    };

    ImageType imageType = ImageType::WIC;

    auto extention_pos = path.find_last_of('.');
    if (extention_pos != std::string::npos)
    {
        auto extention = path.substr(extention_pos);

        if (extention == ".tga" || extention == ".TGA")
        {
            imageType = ImageType::TGA;
        }
    }

    std::wstring widePath;
    Utils::UTF8ToUTF16WString(path, widePath);


    DirectX::ScratchImage image;
    DirectX::TexMetadata metadata;
    switch (imageType)
    {
    case ImageType::TGA:
        graphics->ThrowIfError(DirectX::LoadFromTGAFile(widePath.c_str(), &metadata, image),
                               __FILE__, __LINE__);
        break;

    default:
    case ImageType::WIC:
        graphics->ThrowIfError(DirectX::LoadFromWICFile(widePath.c_str(), DirectX::WIC_FLAGS::WIC_FLAGS_NONE, &metadata, image),
                               __FILE__, __LINE__);
        break;
    }
    
    // create the resource view on the texture
    graphics->ThrowIfError(DirectX::CreateShaderResourceView(graphics->GetDevice(), image.GetImages(), image.GetImageCount(), metadata, &pTextureView),
                           __FILE__, __LINE__);

    

}

void Texture::BindVS(Graphics* graphics, UINT slot)
{
    graphics->GetInfoLogger()->SetLatest();

    graphics->GetContext()->VSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());

    graphics->GetInfoLogger()->DumpIfAny(nodec::logging::Level::Warn);
}

void Texture::BindPS(Graphics* graphics, UINT slot)
{
    graphics->GetInfoLogger()->SetLatest();

    graphics->GetContext()->PSSetShaderResources(slot, 1u, pTextureView.GetAddressOf());

    graphics->GetInfoLogger()->DumpIfAny(nodec::logging::Level::Warn);
}
