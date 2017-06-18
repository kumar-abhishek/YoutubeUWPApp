//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
#include "Helpers.h"

namespace App1
{
	public enum class NotifyType
	{
		StatusMessage,
		ErrorMessage
	};
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

	private:
		MainPage^ rootPage;
		Windows::Web::Http::HttpClient^ httpClient;
		void Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void UpdateStatus(Platform::String^ strMessage, NotifyType type);

	internal:
		static MainPage^ Current;
		void NotifyUser(Platform::String^ strMessage, NotifyType type);
		bool TryGetUri(Platform::String^ uriString, Windows::Foundation::Uri^* uri);

	};
}
