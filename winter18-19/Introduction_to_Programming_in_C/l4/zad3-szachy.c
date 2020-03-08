#include<stdio.h>
char c,t[300][300];
long long licz=0;
int pozy,pozx;
char ratunek;
const int dod=10;
void funkcja(int x,int y);
void wieza (int y,int x);
void krol (int y,int x);
void hetman (int y,int x);
void goniec (int y,int x);
void skoczek (int y,int x);
void pion (int y,int x);
int szachowany (int y,int x);
int main()
{
	for(int g=7;g>=0;g--)
		for(int h=7;h>=0;h--)
		{
			c=getchar();
			if(c=='\n')
				c=getchar();
			if(c=='K')
			{
				pozy=g+dod;
				pozx=h+dod;
			}
			t[g+dod][h+dod]=c;
		}
	for(int g=7;g>=0;g--)
	{
		for(int h=7;h>=0;h--)
		{
            //printf("%c",t[g+dod][h+dod]);
			if(t[g+dod][h+dod]!='.')
			{
				funkcja(g+dod,h+dod);
                //printf("%c",t[g+dod][h+dod]);
                //printf("%lld\n",licz);
            }
        }
       // printf("\n");
    }
    printf("%lld",licz);
    /*
    for(int g=7;g>=0;g--)
	{
		for(int h=7;h>=0;h--)
		{
            printf("%c",t[g+dod][h+dod]);
        }
        printf("\n");
    }
    */
}
void funkcja(int x,int y)
{
	switch(t[x][y])
	{
		case 'W':
			wieza(x,y);
			break;
		case 'S':
			skoczek(x,y);
			break;
		case 'G':
			goniec(x,y);
			break;
		case 'K':
			krol(x,y);
			break;
		case 'H':
			hetman(x,y);
			break;
		case 'P':
			pion(x,y);
			break;
	}
	return;
}
void wieza(int y,int x)
{
	t[y][x]='.';
	int zm1=y,zm2=x+1;

	while(zm2<8+dod)
	{
		if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
			break;
		if(t[zm1][zm2]>='a' && t[zm1][zm2]<='z')
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='W';
			if(szachowany(pozy,pozx))
				licz++;
			t[zm1][zm2]=ratunek;
            break;
		}
		else
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='W';
			if(szachowany(pozy,pozx))
				licz++;
			t[zm1][zm2]=ratunek;
		}
		zm2++;
	}
	zm2=x-1;
	while(zm2>=dod)
	{
		if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
			break;
        if(t[zm1][zm2]>='a' && t[zm1][zm2]<='z')
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='W';
			if(szachowany(pozy,pozx))
				licz++;
			t[zm1][zm2]=ratunek;
            break;
		}
		else
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='W';
			if(szachowany(pozy,pozx))
				licz++;
			t[zm1][zm2]=ratunek;
		}
		zm2--;
	}
	zm2=x;
	zm1=y+1;
	while(zm1<8+dod)
	{
		if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
			break;
		if(t[zm1][zm2]>='a' && t[zm1][zm2]<='z')
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='W';
			if(szachowany(pozy,pozx))
				licz++;
			t[zm1][zm2]=ratunek;
            break;
		}
		else
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='W';
			if(szachowany(pozy,pozx))
				licz++;
			t[zm1][zm2]=ratunek;
		}
		zm1++;
	}
	zm1=y-1;
	while(zm1>=dod)
	{
		if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
			break;
		if(t[zm1][zm2]>='a' && t[zm1][zm2]<='z')
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='W';
			if(szachowany(pozy,pozx))
				licz++;
			t[zm1][zm2]=ratunek;
            break;
		}
		else
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='W';
			if(szachowany(pozy,pozx))
				licz++;
			t[zm1][zm2]=ratunek;
		}
		zm1--;
	}
	t[y][x]='W';
	return;
}
void skoczek(int y,int x)
{
	int zm1=y,zm2=x;
	t[y][x]='.';
	zm2=x+2;
	zm1=y+1;
	if(zm1<8+dod && zm2<8+dod)
	{
		if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
        {
        }
		else
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='S';
			if(szachowany(pozy,pozx))
				licz++;
			t[zm1][zm2]=ratunek;
		}
	}
	zm2=x+2;
	zm1=y-1;
	if(zm1>=dod && zm2<8+dod)
	{
		if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
		{
		}
		else
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='S';
			if(szachowany(pozy,pozx))
				licz++;
			t[zm1][zm2]=ratunek;
		}
	}
	zm2=x-2;
	zm1=y+1;
	if(zm1<8+dod && zm2>=dod)
	{
		if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
		{
		}
		else
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='S';
			if(szachowany(pozy,pozx))
				licz++;
			t[zm1][zm2]=ratunek;
		}
	}
	zm2=x-2;
	zm1=y-1;
	if(zm1>=dod && zm2>=dod)
	{
		if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
		{
		}
		else
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='S';
			if(szachowany(pozy,pozx))
				licz++;
			t[zm1][zm2]=ratunek;
		}
	}
	zm2=x+1;
	zm1=y+2;
	if(zm1<8+dod && zm2<8+dod)
	{
		if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
		{
		}
		else
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='S';
			if(szachowany(pozy,pozx))
				licz++;
			t[zm1][zm2]=ratunek;
		}
	}
	zm2=x+1;
	zm1=y-2;
	if(zm1>=dod && zm2<8+dod)
	{
		if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
		{
		}
		else
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='S';
			if(szachowany(pozy,pozx))
				licz++;
			t[zm1][zm2]=ratunek;
		}
	}
	zm2=x-1;
	zm1=y+2;
	if(zm1<8+dod && zm2>=dod)
	{
		if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
		{
		}
		else
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='S';
			if(szachowany(pozy,pozx))
				licz++;
			t[zm1][zm2]=ratunek;
		}
	}
	zm2=x-1;
	zm1=y-2;
	if(zm1>=dod && zm2>=dod)
	{
		if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
		{
		}
		else
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='S';
			if(szachowany(pozy,pozx))
				licz++;
			t[zm1][zm2]=ratunek;
		}
	}
	t[y][x]='S';
	return;
}
void goniec(int y,int x)
{
	t[y][x]='.';
	int zm1=y+1,zm2=x+1;
	while(zm1<dod+8 && zm2<dod+8)
	{
		if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
			break;
        if(t[zm1][zm2]>='a' && t[zm1][zm2]<='z')
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='W';
			if(szachowany(pozy,pozx))
				licz++;
			t[zm1][zm2]=ratunek;
            break;
		}
		else
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='G';
			if(szachowany(pozy,pozx))
				licz++;
			t[zm1][zm2]=ratunek;
		}
		zm1++;
		zm2++;
	}
	zm1=y-1,zm2=x-1;
	while(zm1>=dod && zm2>=dod)
	{
		if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
			break;
        if(t[zm1][zm2]>='a' && t[zm1][zm2]<='z')
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='W';
			if(szachowany(pozy,pozx))
				licz++;
			t[zm1][zm2]=ratunek;
            break;
		}
		else
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='G';
			if(szachowany(pozy,pozx))
				licz++;
			t[zm1][zm2]=ratunek;
		}
		zm1--;
		zm2--;
	}
	zm1=y-1,zm2=x+1;
	while(zm1>=dod && zm2<dod+8)
	{
		if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
			break;
        if(t[zm1][zm2]>='a' && t[zm1][zm2]<='z')
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='W';
			if(szachowany(pozy,pozx))
				licz++;
			t[zm1][zm2]=ratunek;
            break;
		}
		else
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='G';
			if(szachowany(pozy,pozx))
				licz++;
			t[zm1][zm2]=ratunek;
		}
		zm1--;
		zm2++;
	}
	zm1=y+1,zm2=x-1;
	while(zm2>=dod && zm1<dod+8)
	{
		if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
			break;
        if(t[zm1][zm2]>='a' && t[zm1][zm2]<='z')
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='W';
			if(szachowany(pozy,pozx))
				licz++;
			t[zm1][zm2]=ratunek;
            break;
		}
		else
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='G';
			if(szachowany(pozy,pozx))
				licz++;
			t[zm1][zm2]=ratunek;
		}
		zm1++;
		zm2--;
	}
	t[y][x]='G';
	return;
}
void krol(int y,int x)
{
	t[y][x]='.';
	int zm1=y+1,zm2=x;
	if(zm1<8+dod)
	{
		if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
		{
		}
		else
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='K';
			if(szachowany(zm1,zm2))
				licz++;
			t[zm1][zm2]=ratunek;
		}
	}
	zm1=y,zm2=x+1;
	if(zm2<8+dod)
	{
		if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
		{
		}
		else
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='K';
			if(szachowany(zm1,zm2))
				licz++;
			t[zm1][zm2]=ratunek;
		}
	}
	zm1=y+1,zm2=x+1;
	if(zm1<8+dod && zm2<8+dod)
	{
		if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
		{
		}
		else
		{
		ratunek=t[zm1][zm2];
			t[zm1][zm2]='K';
			if(szachowany(zm1,zm2))
				licz++;
			t[zm1][zm2]=ratunek;
		}
	}
	zm1=y+1,zm2=x-1;
	if(zm1<8+dod && zm2>=dod)
	{
		if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
        {
        }
		else
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='K';
			if(szachowany(zm1,zm2))
				licz++;
			t[zm1][zm2]=ratunek;
		}
	}
	zm1=y,zm2=x-1;
	if(zm2>=dod)
	{
		if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
        {
        }
		else
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='K';
			if(szachowany(zm1,zm2))
				licz++;
			t[zm1][zm2]=ratunek;
		}
	}
	zm1=y-1,zm2=x-1;
	if(zm1>=dod && zm2>=dod)
	{
		if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
        {
        }
		else
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='K';
			if(szachowany(zm1,zm2))
				licz++;
			t[zm1][zm2]=ratunek;
		}
	}
    zm1=y-1,zm2=x;
	if(zm1>=dod)
	{
		if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
        {
        }
		else
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='K';
			if(szachowany(zm1,zm2))
				licz++;
			t[zm1][zm2]=ratunek;
		}
	}
	zm1=y-1,zm2=x+1;
	if(zm2<8+dod && zm1>=dod)
	{
		if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
		{
		}
		else
		{
            ratunek=t[zm1][zm2];
			t[zm1][zm2]='K';
			if(szachowany(zm1,zm2))
				licz++;
			t[zm1][zm2]=ratunek;
		}
	}
	t[y][x]='K';
	return;
}
void hetman(int y,int x)
{
	goniec(y,x);
	wieza(y,x);
	t[y][x]='H';
	return;
}
void pion(int y,int x)
{
	t[y][x]='.';
	if(y==dod+1)
		if(t[y+1][x]=='.' && t[y+2][x]=='.')
		{
            ratunek=t[y+2][x];
			t[y+2][x]='P';
			if(szachowany(pozy,pozx))
				licz++;
			t[y+2][x]=ratunek;
		}
	if(t[y+1][x]=='.')
	{
        ratunek=t[y+1][x];
		t[y+1][x]='P';
		if(szachowany(pozy,pozx))
		{
			if(y+1==7+dod)
				licz+=4;
			else
				licz++;
		}
		t[y+1][x]=ratunek;
	}
	if(x+1<dod+8)
	{
		if(t[y+1][x+1]>='a' && t[y+1][x+1]<='z')
		{
            ratunek=t[y+1][x+1];
			t[y+1][x+1]='P';
			if(szachowany(pozy,pozx))
			{
                if(y+1==7+dod)
                    licz+=4;
                else
                    licz++;
			}
			t[y+1][x+1]=ratunek;
		}
	}
	if(x-1>=dod)
	{
		if(t[y+1][x-1]>='a' && t[y+1][x-1]<='z')
		{
            ratunek=t[y+1][x-1];
			t[y+1][x-1]='P';
			if(szachowany(pozy,pozx))
			{
                if(y+1==7+dod)
                    licz+=4;
                else
                    licz++;
			}
			t[y+1][x-1]=ratunek;
		}
	}
	t[y][x]='P';
	return;
}
int szachowany(int y,int x)
{
    int wsk=0;
    if(t[y-1][x-1]=='p' || t[y-1][x+1]=='p')
        return 0;
    if(t[y][x+1]=='k' || t[y+1][x+1]=='k' || t[y-1][x+1]=='k' || t[y+1][x]=='k' || t[y-1][x]=='k' || t[y][x-1]=='k' || t[y-1][x-1]=='k' || t[y+1][x-1]=='k')
        return 0;
    if(t[y+2][x+1]=='s' || t[y+2][x-1]=='s' || t[y-2][x+1]=='s' || t[y-2][x-1]=='s' || t[y+1][x+2]=='s' || t[y-1][x+2]=='s' || t[y+1][x-2]=='s' || t[y-1][x-2]=='s')
        return 0;
    int zm1=y,zm2=x+1;
    while(zm2<dod+8)
    {
        if(t[zm1][zm2]=='w' || t[zm1][zm2]=='h')
        {
            wsk=1;
            break;
        }
        if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
            break;
        zm2++;
    }
    if(wsk==1)
        return 0;
    zm2=x-1;
    while(zm2>=dod)
    {
        if(t[zm1][zm2]=='w' || t[zm1][zm2]=='h')
        {
            wsk=1;
            break;
        }
        if(t[zm1][zm2]>='A'&& t[zm1][zm2]<='Z')
            break;
        zm2--;
    }
    if(wsk==1)
        return 0;
    zm2=x;
    zm1=y-1;
    while(zm1>=dod)
    {
        if(t[zm1][zm2]=='w' || t[zm1][zm2]=='h')
        {
            wsk=1;
            break;
        }
        if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
            break;
        zm1--;
    }
    if(wsk==1)
        return 0;
    zm1=y+1;
    while(zm1<8+dod)
    {
        if(t[zm1][zm2]=='w' || t[zm1][zm2]=='h')
        {
            wsk=1;
            break;
        }
        if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
            break;
        zm1++;
    }
    if(wsk==1)
        return 0;
    zm2=x+1;
    zm1=y+1;
    while(zm2<8+dod && zm1<8+dod)
    {
        if(t[zm1][zm2]=='g' || t[zm1][zm2]=='h')
        {
            wsk=1;
            break;
        }
        if(t[zm1][zm2]>='A'&& t[zm1][zm2]<='Z')
            break;
        zm2++;
        zm1++;
    }
    if(wsk==1)
        return 0;
    zm2=x-1;
    zm1=y-1;
    while(zm2>=dod && zm1>=dod)
    {
        if(t[zm1][zm2]=='g' || t[zm1][zm2]=='h')
        {
            wsk=1;
            break;
        }
        if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
            break;
        zm2--;
        zm1--;
    }
    if(wsk==1)
        return 0;
    zm2=x+1;
    zm1=y-1;
    while(zm2<8+dod && zm1>=dod)
    {
        if(t[zm1][zm2]=='g' || t[zm1][zm2]=='h')
        {
            wsk=1;
            break;
        }
        if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
            break;
        zm2++;
        zm1--;
    }
    if(wsk==1)
        return 0;
    zm2=x-1;
    zm1=y+1;
    while(zm2>=dod && zm1<8+dod)
    {
        if(t[zm1][zm2]=='g' || t[zm1][zm2]=='h')
        {
            wsk=1;
            break;
        }
        if(t[zm1][zm2]>='A' && t[zm1][zm2]<='Z')
            break;
        zm2--;
        zm1++;
    }
    if(wsk==1)
        return 0;
    return 1;
}

