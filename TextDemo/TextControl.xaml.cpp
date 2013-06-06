//
// TextControl.xaml.cpp
// TextControl 类的实现
//

#include "pch.h"
#include "TextControl.xaml.h"
#include "TextCanvasControl.xaml.h"
#include "TextLayoutItem.xaml.h"
#include "MainPage.xaml.h"

using namespace TextDemo;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Storage::Streams;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::Graphics::Imaging;
using namespace Concurrency;

// “用户控件”项模板在 http://go.microsoft.com/fwlink/?LinkId=234236 上提供

void ApplyText(Array<BYTE>^ mask,Array<BYTE>^ dst,int width,int height) 
{
	for(int i = 0;i < height;i++)
	{
		unsigned char* pMask = mask->Data + i * width * 4;		
		unsigned char* pDst = dst->Data + i * width * 4;
		for(int j = 0;j < width;j++)
		{
			int am,bm,gm,rm,ad,bd,gd,rd,r,g,b;
			am = pMask[3];
			rm = pMask[0];
			gm = pMask[1];
			bm = pMask[2];
			
			ad = pDst[3];
			rd = pDst[0];
			gd = pDst[1];
			bd = pDst[2];			
			
			if(am == 0)
			{						
				pDst += 4;
				pMask += 4;
				continue;
			}
			if(am == 0xff)
			{
				pDst[0] = rm;
				pDst[1] = gm;
				pDst[2] = bm;
				pDst[3] = am;
						
				pDst += 4;
				pMask += 4;
				continue;
			}
			am = am + (am >> 7);
			r = (rd * (256 - am) >> 8) +  rm;
			g = (gd * (256 - am) >> 8) +  gm;
			b = (bd * (256 - am) >> 8) +  bm;
			
			pDst[0] = r;
			pDst[1] = g;
			pDst[2] = b;
					
			pDst += 4;
			pMask += 4;

		}
	}
}

TextControl::TextControl(MainPage^ page)
{
	InitializeComponent();
	m_pAdjustPage = page;
}

TextAttribute^ TextControl::createTextAttribute()
{
	auto attri = ref new TextAttribute();
	attri->textContent = L"Please input text";
	attri->size = 56;
	attri->textFamily=L"Georgia";
	attri->style = TextDemo::FontStyle::STYLE_NORMAL;

	attri->color = Windows::UI::Colors::Blue;
	attri->colorIndex = 0;
	attri->angle = 0;

	attri->alpha = 100;
	attri->scale = 1.0;

	return attri;
}

void TextDemo::TextControl::onAddClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	m_currentTextItem = ref new TextLayoutItem(this);
	m_pAdjustPage->getTextCanvasControl()->addTextLayoutItem(m_currentTextItem);
}


void TextDemo::TextControl::onSaveClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto resultData = m_pAdjustPage->getTextCanvasControl()->getResultData();
	auto previewImg = m_pAdjustPage->getPreview();	
	previewImg->Invalidate();
	
	if(resultData != nullptr && resultData->Length > 0)
	{
		FileSavePicker^ savePicker = ref new FileSavePicker();
		savePicker->SuggestedStartLocation = PickerLocationId::PicturesLibrary;
		auto jpgImageExtensions = ref new Platform::Collections::Vector<String^>();
		jpgImageExtensions->Append(".jpg");		
		savePicker->FileTypeChoices->Insert("jpeg Image", jpgImageExtensions);	

		auto bmpImageExtensions = ref new Platform::Collections::Vector<String^>();
		bmpImageExtensions->Append(".bmp");		
		savePicker->FileTypeChoices->Insert("bmp Image", bmpImageExtensions);	

		auto pngImageExtensions = ref new Platform::Collections::Vector<String^>();
		pngImageExtensions->Append(".png");		
		savePicker->FileTypeChoices->Insert("png Image", pngImageExtensions);

		auto tifImageExtensions = ref new Platform::Collections::Vector<String^>();
		tifImageExtensions->Append(".tiff");		
		savePicker->FileTypeChoices->Insert("tiff Image", tifImageExtensions);

		savePicker->SuggestedFileName = "New Image";	
		
		create_task(savePicker->PickSaveFileAsync()).then([this,resultData,previewImg](StorageFile^ file){
			if(file != nullptr)
			{
				CachedFileManager::DeferUpdates(file);
				// Prevent updates to the remote version of the file until we finish making changes and call CompleteUpdatesAsync.
				create_task(file->OpenAsync(FileAccessMode::ReadWrite)).then([this,file](IRandomAccessStream^ stream){
					stream->Size = 0;
					String^ name =  file->Name;
					String^ type = file->FileType;
					Guid encoderid =  BitmapEncoder::JpegEncoderId;
					if(type->Equals(".bmp"))
					{
						encoderid =  BitmapEncoder::BmpEncoderId;
					}
					else if(type->Equals(".png"))
					{
						encoderid =  BitmapEncoder::PngEncoderId;
					}
					else if(type->Equals(".tiff"))
					{
						encoderid =  BitmapEncoder::TiffEncoderId;
					}						
					auto bitmapPropertySet = ref new BitmapPropertySet();
					auto qualityValue = ref new BitmapTypedValue(1.0,PropertyType::Single);
					if(encoderid == BitmapEncoder::JpegEncoderId)
						bitmapPropertySet->Insert(L"ImageQuality",qualityValue);
					return BitmapEncoder::CreateAsync(encoderid,stream,bitmapPropertySet);
				}).then([this,resultData,previewImg](BitmapEncoder^ enc){
					int pxWidth = m_pAdjustPage->getTextCanvasControl()->getResultWidth();
					int pxHeight = m_pAdjustPage->getTextCanvasControl()->getResultHeight();
									
					DataReader^ reader = DataReader::FromBuffer(previewImg->PixelBuffer);
					auto len = reader->UnconsumedBufferLength;
					
					reader = DataReader::FromBuffer(previewImg->PixelBuffer);
					len = reader->UnconsumedBufferLength;

					auto imgData = ref new Array<byte>(previewImg->PixelBuffer->Length);
					reader->ReadBytes(imgData);
					ApplyText(resultData,imgData,pxWidth,pxHeight);
					enc->SetPixelData(BitmapPixelFormat::Bgra8,BitmapAlphaMode::Ignore,pxWidth,pxHeight,96,96,imgData);
					return enc->FlushAsync();
				}).then([this,file](){
					return CachedFileManager::CompleteUpdatesAsync(file);
				});
			}
		});
	}
}
