#include<iostream>
#include<windows.h> //potrzebne do Sleep(),
					//do ustawiania cursora,
					//do CONSOLE_SCREEN_BUFFER_INFOEX i pozosta³ych funkcji zwi¹zanych ze zmian¹ kolorów,
					//do CONSOLE_SCREEN_BUFFER_INFO - iloœci kolumn i rzêdów
#include<conio.h>//do _kbhit() i _getch()

using namespace std;

const int OY = 12, OX = 18;
char plansza[OY][OX] =
//     osie: OY  OX
{
	{"#################"},
	{"#               #"},
	{"#               #"},
	{"#               #"},
	{"#               #"},
	{"#               #"},
	{"#               #"},
	{"#               #"},
	{"#               #"},
	{"#               #"},
	{"#               #"},
	{"#################"} //10 x 15
};						  //150 pól

//tu zapisujê ruch podany przez gracza
char ruch;
char klawisz_góra{ 'w' }, klawisz_dó³{ 's' }, klawisz_lewo{ 'a' }, klawisz_prawo{ 'd' };

enum Kierunki { lewo = 1, góra, dó³, prawo };
Kierunki kierunek = prawo, poprzedni = prawo;


int w¹¿X[150]; //tyle elementów tablicy ile mo¿liwych pól
int w¹¿Y[150];

//wonszX[i] -> wspó³rzêdna X dla konkretnej "czêœci" wê¿a 
//wonszX[0]	-> O -> g³ówka wê¿a

//pocz¹tkowa d³ugoœæ czyli tylko g³owa
int d³{ 0 };

enum szybkoœæ { wolny = 1, œredni, szybki };
szybkoœæ szybkoœæ_wê¿a = œredni, szybkoœæ_poprzednia = wolny;

//--------------------------------------------------------------------
//koordynaty dla jab³ka
int xj, yj;

//czy w¹¿ zjad³ jab³ko?
bool am = false;

char œ = 0x98, ¹ = 0xa5, æ = 0x86, ê = 0xa9, ¿ = 0xbe, ³ = 0x88, Ÿ = 0xab, ñ = 0xe4, ó = 0xa2;
char lewa_strza³ka = 0x11, prawa_strza³ka = 0x10;

char naciœnij;

int czekaæ; //do Sleepa

CONSOLE_SCREEN_BUFFER_INFO csbi;
int kolumny;

COORD c; //do zmieniania pozycji kursora

/*-------------------------------------------------------------------------------*/
/*									 FUNKCJE									 */
/*-------------------------------------------------------------------------------*/

void intro()
{
	//kolory
	CONSOLE_SCREEN_BUFFER_INFOEX info1;
	info1.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);
	
	HANDLE hConsole1 = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfoEx(hConsole1, &info1);

	info1.ColorTable[0] = RGB(0, 0, 0);//ten kolor jest t³em
	info1.ColorTable[1] = RGB(0, 135, 3); //do napisu SNAKE
	info1.ColorTable[2] = RGB(200, 200, 200);//powrót do bia³ego

	SetConsoleScreenBufferInfoEx(hConsole1, &info1);
	//---------------------------------------------------------------
	do
	{
		//ile kolumn?
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		kolumny = csbi.srWindow.Right - csbi.srWindow.Left + 1; //srWindow zawiera koordynaty lewego górnego i prawego dolnego rogu

		system("cls");
		cout << endl << endl;
		cout.width((kolumny / 2) - 12);
		SetConsoleTextAttribute(hConsole1, 1);
		cout << " THE" << endl << endl;

		SetConsoleTextAttribute(hConsole1, 1);
		cout.width((kolumny / 2) + 16);
		cout << "# # #  #   #    #    #  #  # # #" << endl;
		cout.width((kolumny / 2) + 16);
		cout << "#      ##  #  #   #  # #   #    " << endl;
		cout.width((kolumny / 2) + 16);
		cout << "# # #  # # #  # # #  ##    # #  " << endl;
		cout.width((kolumny / 2) + 16);
		cout << "    #  #  ##  #   #  # #   #    " << endl;
		cout.width((kolumny / 2) + 16);
		cout << "# # #  #   #  #   #  #  #  # # #" << endl << endl;

		cout.width((kolumny / 2) + 16);
		cout << "GAME " << endl << endl;
		
		SetConsoleTextAttribute(hConsole1, 2);
		cout.width((kolumny / 2) + 16);
		cout << "       by Szymon Jakubiec       " << endl << endl;

		cout.width((kolumny / 2) - 10);
		cout << "  naci" << œ << "nij ENTER aby rozpocz" << ¹ << æ << endl << endl;

		naciœnij = _getch();
		
	} while (naciœnij != 13);
}

void opcje()
{
	do
	{		
		system("cls");
		cout << endl << endl;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		kolumny = csbi.srWindow.Right - csbi.srWindow.Left + 1; //srWindow zawiera koordynaty lewego górnego i prawego dolnego rogu

		cout.width((kolumny / 2) - 18);
		cout << "zmie" << ñ << " szybko" << œ << æ << " w" << ê << ¿ << "a u" << ¿ << "ywaj" << ¹ << "c klawiszy (a,d)" << endl;


		cout.width((kolumny / 2) - 9);
		cout << "i zatwierd" << Ÿ << " klawiszem ENTER by zagra" << æ << endl << endl;
		

		cout << "\r";
		cout.width((kolumny / 2) - 7);

		if (szybkoœæ_wê¿a == œredni)
		{
			cout << "szybko" << œ << æ << " w" << ê << ¿ << "a:   " << lewa_strza³ka << œ << "redni" << prawa_strza³ka;
		}
		else if (szybkoœæ_wê¿a == wolny)
		{
			cout << "szybko" << œ << æ << " w" << ê << ¿ << "a:   " << " wolny" << prawa_strza³ka << " ";
		}
		else if (szybkoœæ_wê¿a == szybki)
		{
			cout << "szybko" << œ << æ << " w" << ê << ¿ << "a:   " << lewa_strza³ka << "szybki ";
		}
		
		
		naciœnij = _getch();
		if ((naciœnij == 'a') || (naciœnij == 'd') || (naciœnij == 13))
		{
			if (szybkoœæ_wê¿a == œredni)
			{
				if (naciœnij == 'a')		szybkoœæ_wê¿a = wolny;
				else if (naciœnij == 'd')	szybkoœæ_wê¿a = szybki;
			}
			else if (szybkoœæ_wê¿a == wolny)
			{
				if (naciœnij == 'd')	szybkoœæ_wê¿a = œredni;
			}
			else if (szybkoœæ_wê¿a == szybki)
			{
				if (naciœnij == 'a')	szybkoœæ_wê¿a = œredni;
			}
		}
	} while (naciœnij != 13);
}


int main()
{
	/*----------------------------------------------------------------------------------------------*/
	/*											  WSTÊP 											*/
	/*----------------------------------------------------------------------------------------------*/

	intro();

	opcje();

	if (szybkoœæ_wê¿a == wolny)				czekaæ = 200;
	else if (szybkoœæ_wê¿a == œredni)		czekaæ = 150;
	else if (szybkoœæ_wê¿a == szybki)		czekaæ = 100;
	
	/*----------------------------------------------------------------------------------------------*/
	/*											 PRZED GR¥											*/
	/*----------------------------------------------------------------------------------------------*/
			
	system("cls");

	//kolory
	CONSOLE_SCREEN_BUFFER_INFOEX info2;
	info2.cbSize = sizeof(CONSOLE_SCREEN_BUFFER_INFOEX);

	HANDLE hConsole2 = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfoEx(hConsole2, &info2);

	info2.ColorTable[0] = RGB(128, 229, 122);	//t³o rozgrywki
	info2.ColorTable[1] = RGB(20, 92, 20);		//kolor krzaków
	info2.ColorTable[2] = RGB(225, 126, 66);	//jab³ko
	info2.ColorTable[3] = RGB(125, 93, 44);		//w¹¿

	SetConsoleScreenBufferInfoEx(hConsole2, &info2);

	//wypisanie planszy
	SetConsoleTextAttribute(hConsole2, 1); //w kolorze krzaków

	for (int i = 0; i < OY; i++)
		cout << *(plansza + i) << endl;
	
	//wypisanie punktów
	SetConsoleTextAttribute(hConsole2, 3); //by odró¿nia³y siê kolorem od krzaków
	cout << "punty:\t" << d³;

	//ustawienie pierwszego jab³ka
	do
	{
		xj = rand() % (OX - 3) + 1; //(1,17)		
		yj = rand() % (OY - 2) + 1;//(1,8)

		SetConsoleTextAttribute(hConsole2, 1);
	} while (plansza[yj][xj] != ' ');

	plansza[yj][xj] = '+';

	c.X = xj;
	c.Y = yj;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
	SetConsoleTextAttribute(hConsole2, 2);//kolor jab³ka
	cout << '+';

	//startowe ustawienie wê¿a
	w¹¿X[0] = 5;
	w¹¿Y[0] = 5;

	/*--------------------------------------------------------------------------------------------------*/
	/*												   GRA												*/
	/*--------------------------------------------------------------------------------------------------*/

	while (d³ < 149)
	{
		/*-------------------------   wype³nienie planszy wê¿em   --------------------------*/
		
		//g³owa wê¿a ma dl=0
		plansza[w¹¿Y[0]][w¹¿X[0]] = 'O';

		c.X = w¹¿X[0];
		c.Y = w¹¿Y[0];
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
		SetConsoleTextAttribute(hConsole2, 3);//kolor wê¿a
		cout << 'O';

		if (d³ > 0)
		{
			c.X = w¹¿X[1];
			c.Y = w¹¿Y[1];
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
			cout << 'o';
		}

		/*----------------------------------------------------------------------------------*/
		/*										  ruch										*/
		/*----------------------------------------------------------------------------------*/
		
		//sztucznie przerzucam wskaŸnik w miejsce iloœci punktów by nie przeszkadza³ przy wê¿u
		if (d³ > 9)
			c.X = 10;
		else if (d³ > 99)
			c.X = 11;
		else
			c.X = 9;
		c.Y = OY;
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
		Sleep(czekaæ);		

		//tu wykrywam czy jakiœ klawisz zosta³ wciœniêty
		if (_kbhit())
		{
			ruch = _getch();
			if ((poprzedni == lewo) || (poprzedni == prawo))
			{
				if (ruch == klawisz_góra)		kierunek = góra;
				else if (ruch == klawisz_dó³)	kierunek = dó³;
			}
			else if ((poprzedni == góra) || (poprzedni == dó³))
			{
				if (ruch == klawisz_lewo)		kierunek = lewo;
				else if (ruch == klawisz_prawo)	kierunek = prawo;
			}
		}


		if (am == false)
		{
			//"koniec" wê¿a usuwa siê z ostatniego miejsca 
			plansza[w¹¿Y[d³]][w¹¿X[d³]] = ' ';

			c.X = w¹¿X[d³];
			c.Y = w¹¿Y[d³];
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
			cout << ' ';

			for (int i = d³; i > 0; i--)
			{
				w¹¿X[i] = w¹¿X[i - 1];
				w¹¿Y[i] = w¹¿Y[i - 1];
			}
		}
		else //czyli jak zjad³ jab³ko
		{
			am = false;

			//w¹¿ wyd³u¿a siê w miejscu, które opuszcza dotychczasowy "koniec" wê¿a
			d³++;
			w¹¿X[d³] = w¹¿X[d³ - 1];
			w¹¿Y[d³] = w¹¿Y[d³ - 1];
			plansza[w¹¿Y[d³]][w¹¿X[d³]] = 'o';

			c.X = w¹¿X[d³];
			c.Y = w¹¿Y[d³];
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
			SetConsoleTextAttribute(hConsole2, 3);//kolor wê¿a
			cout << 'o';

			//update wypisu iloœci punktów
			c.X = 8;
			c.Y = OY;
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
			cout << d³;

			for (int i = d³ - 1; i > 0; i--)
			{
				w¹¿X[i] = w¹¿X[i - 1];
				w¹¿Y[i] = w¹¿Y[i - 1];
			}

			/*-------------------   ustawienie kolejnego jab³ka	  -------------------*/
			{
				do
				{
					xj = rand() % (OX - 3) + 1; //(1,17)		
					yj = rand() % (OY - 2) + 1;//(1,8)

				} while (plansza[yj][xj] != ' ');
				plansza[yj][xj] = '+';

				//wypis 
				c.X = xj;
				c.Y = yj;
				SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
				SetConsoleTextAttribute(hConsole2, 2);//kolor jab³ka
				cout << '+';
			}
		}

		/*---------------------		 w któr¹ stronê g³owa wê¿a?		----------------------------*/
		if (kierunek == góra)
		{
			if (plansza[w¹¿Y[0] - 1][w¹¿X[0]] == ' ')
			{
				w¹¿Y[0]--;
			}
			else if (plansza[w¹¿Y[0] - 1][w¹¿X[0]] == '+')
			{
				w¹¿Y[0]--;
				am = true; //w¹¿ zjada jab³ko
			}
			else break;
		}
		//---------------------------------------------------------------
		else if (kierunek == dó³)
		{
			if (plansza[w¹¿Y[0] + 1][w¹¿X[0]] == ' ')
			{
				w¹¿Y[0]++;
			}
			else if (plansza[w¹¿Y[0] + 1][w¹¿X[0]] == '+')
			{
				w¹¿Y[0]++;
				am = true;
			}
			else break;
		}
		//---------------------------------------------------------------
		else if (kierunek == lewo)
		{
			if (plansza[w¹¿Y[0]][w¹¿X[0] - 1] == ' ')
			{
				w¹¿X[0]--;
			}
			else if (plansza[w¹¿Y[0]][w¹¿X[0] - 1] == '+')
			{
				w¹¿X[0]--;
				am = true;
			}
			else break;
		}
		//---------------------------------------------------------------
		else if (kierunek == prawo)
		{
			if (plansza[w¹¿Y[0]][w¹¿X[0] + 1] == ' ')
			{
				w¹¿X[0]++;
			}
			else if (plansza[w¹¿Y[0]][w¹¿X[0] + 1] == '+')
			{
				w¹¿X[0]++;
				am = true;
			}
			else break;
		}

		poprzedni = kierunek;
	}

	/*----------------------------------------------------------------------------------*/
	/*									 KONIEC GRY										*/
	/*----------------------------------------------------------------------------------*/
	system("cls");

	Sleep(250);

	SetConsoleTextAttribute(hConsole2, 2);
	cout.width(24);
	if (d³ < 149)
		cout << "KONIEC GRY!!!" << endl << endl;//24
	else
		cout << "GRATULACJE!!!" << endl << endl;
	SetConsoleTextAttribute(hConsole2, 3);
	cout << "     zdoby" << ³ << "e" << œ << "(-a" << œ << ") ";
	SetConsoleTextAttribute(hConsole2, 2);
	cout << d³;

	SetConsoleTextAttribute(hConsole2, 3);

	if (d³ == 1)
		cout << " punkt";
	else if ((d³ % 10 == 2) || (d³ % 10 == 3) || (d³ % 10 == 4))
	{
		if ((d³ != 12) && (d³ != 13) && (d³ != 14))
			cout << " punkty";
		else
			cout << " punkt" << ó << "w";
	}
	else
	{
		cout << " punkt" << ó << "w";
	}
	cout << endl << endl;

	cout << "naci" << œ << "nij dowolny klawisz aby wyj" << œ << æ;

	SetConsoleTextAttribute(hConsole2, 0);

	return 0;
}