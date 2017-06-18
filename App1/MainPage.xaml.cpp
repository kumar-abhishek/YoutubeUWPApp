//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

using namespace App1;
using namespace concurrency;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Filters;
using namespace Windows::Security::Cryptography::Certificates;

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


MainPage::MainPage()
{
	InitializeComponent();
}


void App1::MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	Uri^ uri;
	httpClient = ref new HttpClient();
	// The value of 'AddressField' is set by the user and is therefore untrusted input. If we can't create a
	// valid, absolute URI, we'll notify the user about the incorrect input.
	if (!rootPage->TryGetUri(greetingOutput->Text , &uri))
	{
		rootPage->NotifyUser("Invalid URI.", NotifyType::ErrorMessage);
		return;
	}


	// ---------------------------------------------------------------------------
	// WARNING: Only test applications should ignore SSL errors.
	// In real applications, ignoring server certificate errors can lead to MITM
	// attacks (while the connection is secure, the server is not authenticated).
	//
	// The SetupServer script included with this sample creates a server certificate that is self-signed
	// and issued to fabrikam.com, and hence we need to ignore these errors here. 
	// ---------------------------------------------------------------------------
	//filter->IgnorableServerCertificateErrors->Append(ChainValidationResult::Untrusted);
	//filter->IgnorableServerCertificateErrors->Append(ChainValidationResult::InvalidName);


	// Do an asynchronous GET. We need to use use_current() with the continuations since the tasks are completed on
	// background threads and we need to run on the UI thread to update the UI.
	create_task(httpClient->GetAsync(uri) /*, cancellationTokenSource.get_token() */).then([=](HttpResponseMessage^ response)
	{
		return Helpers::DisplayTextResultAsync(response, OutputField /*,cancellationTokenSource.get_token()*/);
	}, task_continuation_context::use_current()).then([=](task<HttpResponseMessage^> previousTask)
	{});
}

void MainPage::NotifyUser(String^ strMessage, NotifyType type)
{
	if (Dispatcher->HasThreadAccess)
	{
		UpdateStatus(strMessage, type);
	}
	else
	{
		Dispatcher->RunAsync(CoreDispatcherPriority::Normal, ref new DispatchedHandler([strMessage, type, this]()
		{
			UpdateStatus(strMessage, type);
		}));
	}
}

void MainPage::UpdateStatus(String^ strMessage, NotifyType type)
{
	switch (type)
	{
	case NotifyType::StatusMessage:
		StatusBorder->Background = ref new SolidColorBrush(Windows::UI::Colors::Green);
		break;
	case NotifyType::ErrorMessage:
		StatusBorder->Background = ref new SolidColorBrush(Windows::UI::Colors::Red);
		break;
	default:
		break;
	}

	StatusBlock->Text = strMessage;

	// Collapse the StatusBlock if it has no text to conserve real estate.
	if (StatusBlock->Text != "")
	{
		StatusBorder->Visibility = Windows::UI::Xaml::Visibility::Visible;
		StatusPanel->Visibility = Windows::UI::Xaml::Visibility::Visible;
	}
	else
	{
		StatusBorder->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
		StatusPanel->Visibility = Windows::UI::Xaml::Visibility::Collapsed;
	}
}


bool MainPage::TryGetUri(Platform::String^ uriString, Windows::Foundation::Uri^* uri)
{
	// Create a Uri instance and catch exceptions related to invalid input. This method returns 'true'
	// if the Uri instance was successfully created and 'false' otherwise.  
	try
	{
		Windows::Foundation::Uri^ localUri = ref new Windows::Foundation::Uri(Helpers::Trim(uriString));

		if (localUri->Host == nullptr)
		{
			return false;
		}

		if (localUri->SchemeName != "http" && localUri->SchemeName != "https")
		{
			return false;
		}

		*uri = localUri;
		return true;
	}
	catch (Platform::NullReferenceException^ exception)
	{
		NotifyUser("Error: URI must not be null or empty.", NotifyType::ErrorMessage);
	}
	catch (Platform::InvalidArgumentException^ exception)
	{
		NotifyUser("Error: Invalid URI", NotifyType::ErrorMessage);
	}
	return false;
}