//
// TextControl.xaml.cpp
// TextControl 类的实现
//

#include "pch.h"
#include "TextControl.xaml.h"
#include "TextCanvasControl.xaml.h"
#include "TextLayoutItem.xaml.h"
#include "GraphicLayoutItem.xaml.h"
#include "MainPage.xaml.h"
#include "DirectXHelper.h"

using namespace TextDemo;

using namespace D2D1;
using namespace Platform;
using namespace Platform::Collections;
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
using namespace Microsoft::WRL;
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

	m_isChinese = false;
	EventLock = false;

	initTextControl();
}

void TextControl::initTextControl()
{
	cob_FontFamily->DataContext = getFontFamily();
	if(cob_FontFamily->Items->Size > 0)
		cob_FontFamily->SelectedIndex = 0;
	cob_Color->DataContext = getColors();
	if(cob_Color->Items->Size > 0)
		cob_Color->SelectedIndex = 0;
}

TextAttribute^ TextControl::createTextAttribute()
{
	auto attri = ref new TextAttribute();
	attri->textContent = TB_Input->Text;
	attri->size = 56;
	attri->textFamily= safe_cast<Windows::UI::Xaml::Media::FontFamily^>(this->cob_FontFamily->SelectedItem)->Source;
	attri->style = getCurrentFontStyle();

	Windows::UI::Color color= safe_cast<TextColors^>(cob_Color->SelectedValue)->Color;
	attri->color = color;
	attri->colorIndex = cob_Color->SelectedIndex;

	attri->textAlignment = TextAlignment::Left;
	attri->itemType = TextItemType::Type_Text;
	/*attri->itemType = TextItemType::Type_Graphics;
	attri->bgimageName = L"GB1.png";*/
	attri->angle = 0;

	attri->alpha = 100 - transparencySlider->Value;
	attri->scale = 1.0;
	attri->charaSpacing = charaSpacingSlider->Value;
	

	return attri;
}

void TextControl::setCurrentItem(ILayoutItem^ item)
{
	if(m_currentTextItem != nullptr)
		m_currentTextItem->showSelectBorder(false);
	m_currentTextItem = item;
	if(item != nullptr)
	{
		item->showSelectBorder(true);
		auto attri = item->getTextAttribute();
		if(attri->itemType == TextItemType::Type_Text)
		{
			auto fontFamily = ref new Windows::UI::Xaml::Media::FontFamily(attri->textFamily);
			cob_FontFamily->SelectedIndex = attri->textFamilyIndex;
			cob_FontFamily->FontFamily = fontFamily;
			cob_Color->SelectedIndex = attri->colorIndex;
			if(attri->textAlignment == TextAlignment::Left)
				align_Left->IsChecked = true;
			else if(attri->textAlignment == TextAlignment::Center)
				align_Center->IsChecked = true;
			else if(attri->textAlignment == TextAlignment::Right)
				align_Right->IsChecked = true;
			setCurrentFontStyle(attri->style);
			transparencySlider->Value = 100 - attri->alpha;
			charaSpacingSlider->Value = attri->charaSpacing;
			TB_Input->Text = attri->textContent;
		}

	}
	else
	{
		cob_FontFamily->SelectedIndex = 0;
		cob_FontFamily->FontFamily = safe_cast<Windows::UI::Xaml::Media::FontFamily^>(cob_FontFamily->SelectedItem);
		cob_Color->SelectedIndex = 0;
		align_Left->IsChecked = true;
		TB_Input->Text = L"Please input text";
	}
		

}

void TextControl::deleteTextItem(ILayoutItem^ item)
{
	m_pAdjustPage->getTextCanvasControl()->removeTextLayoutItem(item);
	setCurrentItem(nullptr);
}

Vector<TextColors^>^ TextControl::getColors()
{
	auto resultVector=ref new Platform::Collections::Vector<TextColors^>();
	int colorValue[23] = {
		0xffffff,
		0x424242,
		0x000000,
		0xA4C400,
		0x60A917,
		0x008A00,
		0x00ABA9,
		0x1BA1E2,
		0x0050EF,
		0x6A00FF,
		0xAA00FF,
		0xF472D0,
		0xD80073,
		0xA20025,
		0xE51400,
		0xFA6800,
		0xF0A30A,
		0xE3C800,
		0x825A2C,
		0x6D8764,
		0x647687,
		0x76608A,
		0x87794E,
	};	
	int len=sizeof(colorValue)/4;
	for(int i=0;i<len;i++){		
		auto color = ref new TextColors();
		color->Color = Windows::UI::ColorHelper::FromArgb(
			0xff,
			colorValue[i]>>16,
			colorValue[i]>>8&0xFF,
			colorValue[i]&0x0000FF);
		resultVector->Append(color);
	}
	return resultVector;
}

Vector<Windows::UI::Xaml::Media::FontFamily ^>^ TextControl::getFontFamily()
{
	auto resultVector = ref new Vector<Windows::UI::Xaml::Media::FontFamily^>();
	ComPtr<IDWriteFactory> pDWriteFactory;
	ComPtr<IDWriteFontCollection> pFontCollection;

	HRESULT hr = DWriteCreateFactory(
            DWRITE_FACTORY_TYPE_SHARED,
            __uuidof(IDWriteFactory),
            &pDWriteFactory
            );

	if (SUCCEEDED(hr))
    {
		pDWriteFactory->GetSystemFontCollection(&pFontCollection);
	}

	 UINT32 familyCount = 0;

    // Get the number of font families in the collection.
    if (SUCCEEDED(hr))
    {
        familyCount = pFontCollection->GetFontFamilyCount();
    }
	
    for (UINT32 i = 0; i < familyCount; ++i)
    {
        ComPtr<IDWriteFontFamily> pFontFamily;

        // Get the font family.
        if (SUCCEEDED(hr))
        {
            hr = pFontCollection->GetFontFamily(i, &pFontFamily);
        }

        ComPtr<IDWriteLocalizedStrings> pFamilyNames;
        // Get a list of localized strings for the family name.
        if (SUCCEEDED(hr))
        {
            hr = pFontFamily->GetFamilyNames(&pFamilyNames);
        }

        UINT32 index = 0;
        BOOL exists = false;
        
        wchar_t localeName[LOCALE_NAME_MAX_LENGTH];

        if (SUCCEEDED(hr))
        {
            // Get the default locale for this user.
            int defaultLocaleSuccess = GetUserDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH);

            // If the default locale is returned, find that locale name, otherwise use "en-us".
            if (defaultLocaleSuccess)
            {
                hr = pFamilyNames->FindLocaleName(localeName, &index, &exists);
            }
            if (SUCCEEDED(hr) && !exists) // if the above find did not find a match, retry with US English
            {
                hr = pFamilyNames->FindLocaleName(L"en-us", &index, &exists);
            }
        }
        
        // If the specified locale doesn't exist, select the first on the list.
        if (!exists)
            index = 0;

        UINT32 length = 0;

        // Get the string length.
        if (SUCCEEDED(hr))
        {
            hr = pFamilyNames->GetStringLength(index, &length);
        }

        // Allocate a string big enough to hold the name.
		wchar_t* name=new (std::nothrow)wchar_t[length+1];
        if (name == NULL)
        {
            hr = E_OUTOFMEMORY;
        }

        // Get the family name.
        if (SUCCEEDED(hr))
        {
            hr = pFamilyNames->GetString(index, name, length+1);
			
        }
        if (SUCCEEDED(hr))
        {
			
			Platform::String^ tempstr=ref new Platform::String(name);
			auto family = ref new Windows::UI::Xaml::Media::FontFamily(tempstr);
			resultVector->Append(family);			
        }
		
		pFontFamily = nullptr;
		pFamilyNames = nullptr;
        delete [] name;
   }

	pFontCollection = nullptr;
	pDWriteFactory = nullptr;
	//对集合进行排序
	int k;
	int j;
	Windows::UI::Xaml::Media::FontFamily^ tmp;
	for(int i=0;i<resultVector->Size;i++){
		k=i;
		for(j=i+1;j<resultVector->Size;j++){
			if(m_isChinese){
				if(resultVector->GetAt(k)->Source<resultVector->GetAt(j)->Source){
					k=j;
				}
			}else{
				if(resultVector->GetAt(k)->Source>resultVector->GetAt(j)->Source){
					k=j;
				}
			}
		}
		if(k!=i){
			tmp=resultVector->GetAt(k);
			resultVector->SetAt(k,resultVector->GetAt(i));
			resultVector->SetAt(i,tmp);
		}
	}
  return resultVector;

}

void TextDemo::TextControl::onAddClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto item = ref new TextLayoutItem(this,m_pAdjustPage->getTextCanvasControl());
	m_pAdjustPage->getTextCanvasControl()->addTextLayoutItem(item);
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

TextDemo::FontStyle TextControl::getCurrentFontStyle(){
	TextDemo::FontStyle fontStyle= TextDemo::FontStyle::STYLE_NORMAL;
	if(checkBox_Bold->IsChecked->Value){
		fontStyle = fontStyle | TextDemo::FontStyle::STYLE_BOLD;
	}
	if(checkBox_Italic->IsChecked->Value){
		fontStyle = fontStyle | TextDemo::FontStyle::STYLE_ITALIC;
	}
	if(checkBox_undeline->IsChecked->Value){
		fontStyle = fontStyle | TextDemo::FontStyle::STYLE_UNDERLINE;
	}
	return fontStyle;
}

void TextControl::setCurrentFontStyle(TextDemo::FontStyle fontStyle)
{
	if((bool)(fontStyle & TextDemo::FontStyle::STYLE_BOLD)){
		checkBox_Bold->IsChecked=true;
	}else{
		checkBox_Bold->IsChecked=false;
	}
	if((bool)(fontStyle & TextDemo::FontStyle::STYLE_ITALIC)){
		checkBox_Italic->IsChecked=true;
	}else{
		checkBox_Italic->IsChecked=false;
	}
	if((bool)(fontStyle & TextDemo::FontStyle::STYLE_UNDERLINE)){
		checkBox_undeline->IsChecked=true;
	}else{
		checkBox_undeline->IsChecked=false;
	}
}

void TextDemo::TextControl::onSelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	auto comboBox = safe_cast<ComboBox^>(sender);
	if(comboBox->SelectedIndex < 0)
		return;
	if(cob_FontFamily == comboBox)
	{
		cob_FontFamily->FontFamily = safe_cast<Windows::UI::Xaml::Media::FontFamily^>(cob_FontFamily->SelectedItem);
	}
	if(m_currentTextItem == nullptr || EventLock == true)
		return;
	if(cob_FontFamily == comboBox)
	{		
		m_currentTextItem->getTextAttribute()->textFamily = cob_FontFamily->FontFamily->Source;
		m_currentTextItem->getTextAttribute()->textFamilyIndex = cob_FontFamily->SelectedIndex;
		m_currentTextItem->notifyChanged();
	}
	else if(cob_Color == comboBox)
	{
		auto color = safe_cast<TextColors^>(cob_Color->SelectedItem)->Color;
		m_currentTextItem->getTextAttribute()->color = color;
		m_currentTextItem->getTextAttribute()->colorIndex = cob_Color->SelectedIndex;
		m_currentTextItem->notifyChanged();
	}
	
	
}


void TextDemo::TextControl::OnInputTextChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::TextChangedEventArgs^ e)
{
	if(m_currentTextItem == nullptr || EventLock == true)
		return;
	m_currentTextItem->getTextAttribute()->textContent = TB_Input->Text;
	m_currentTextItem->notifyChanged();
	
}


void TextDemo::TextControl::OnAlignChecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if(m_currentTextItem == nullptr || EventLock == true)
		return;
	TextAlignment align;
	auto alignBtn = safe_cast<RadioButton^>(sender);
	if(align_Left == alignBtn)
		align = TextAlignment::Left;
	else if(align_Center == alignBtn)
		align = TextAlignment::Center;
	else if(align_Right == alignBtn)
		align = TextAlignment::Right;
	m_currentTextItem->getTextAttribute()->textAlignment = align;
	m_currentTextItem->notifyChanged();
}


void TextDemo::TextControl::OnTextStyle_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	if(m_currentTextItem == nullptr || EventLock == true)
		return;
	bool isChange=false;
	if(m_currentTextItem->getTextAttribute()->style!=getCurrentFontStyle()){
		isChange=true;
	}
	m_currentTextItem->getTextAttribute()->style=getCurrentFontStyle();
	m_currentTextItem->notifyChanged();
	/*m_pAdjustPage->setNeedSave(true);
	if(isChange)
		saveTextData();*/
}

void TextDemo::TextControl::OnSliderValueChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs^ e)
{
	if(m_currentTextItem == nullptr || EventLock == true)
		return;
	Slider ^slider = safe_cast<Slider^>(sender);
	if(transparencySlider == slider)
	{
		m_currentTextItem->getTextAttribute()->alpha = 100- slider->Value;
		m_currentTextItem->notifyChanged();
	}
	else if(charaSpacingSlider == slider)
	{
		m_currentTextItem->getTextAttribute()->charaSpacing = slider->Value;
		m_currentTextItem->notifyChanged();
	}
}
