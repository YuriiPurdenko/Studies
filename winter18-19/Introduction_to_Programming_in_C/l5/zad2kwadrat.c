#include<stdio.h>
#include<stdbool.h>
bool magkwad(int x,int y[]);
int perm(int a, bool b[], int x,int y, int z[],int wiersz[],int kolum[]);
int licz;
//Wiktor Pilarczyk lista 5 zad 2 308533
int main(int argc, char *argv[])
{
    int n,k,t[100]={0},ta[10]={0},taa[10]={0};
    bool tab[130]={0};
    if(argc==3)
    {
        sscanf(argv[1],"%d",&n);
        sscanf(argv[2],"%d",&k);
        if(n>0 && n<7 && k>=0)
        {
		k=perm(k,tab,0,n,t,ta,taa);
        	if(k>0)
        		printf("Nie ma tylu kwadratow magicznych!\nPozostalo %d nieistniejacych kwadratow do wypisania\n",k);
        }
        else
            printf("Zle argumenty");
    }
    else
        printf("Zla liczba argumentow");

    return 0;
}
int perm(int a, bool b[], int x,int y, int z[],int wiersz[],int kolum[])
{
	int sum=((y*y+1)*y)/2;
	bool zm;
	if(a<=0)
		return a;
	if(x==y*y-y && magkwad(y,z))		//jesli wchodze do ostatniego wierszu, sprawdzam czy przekatne sa ok i wypisuje kwadrat
	{
		a--;
		printf("%d kwadrat: \n",a+1);
		for(int g=0;g<y;g++)
		{
			for(int h=0;h<y;h++)
				printf("%d ",z[g*y+h]);
			printf("\n");
		}
	}
	else
	{
		if((x/y)%2==0)	//sprawdzam czy wiersze sa parzyste, dzieki temu moge zrobic tzw 'wezyka'
		{
			if(x%y==y-1)	//jesli jestem w ostatnim elemencie wiersza moge go wypelnic
			{
				if(b[sum-wiersz[x/y]]==0 && (sum-wiersz[x/y])<=y*y)
				{
					z[x]=sum-wiersz[x/y];
					b[z[x]]=1;
					wiersz[x/y]=sum;
					kolum[y-1]+=z[x];
					if(kolum[y-1]<sum)
					{
						if(x/y==y-2)	//jesli jestem w przedostatnim wierszu uzupelniam ostatni wiersz(jesli moge - suma-(suma na kolumnie))
						{
							if(b[sum-kolum[y-1]]==0)
							{
								b[sum-kolum[y-1]]=1;
								z[x+y]=sum-kolum[y-1];
								a=perm(a,b,x+1,y,z,wiersz,kolum);
								b[sum-kolum[y-1]]=0;
							
							}
						}
						else
							a=perm(a,b,x+1,y,z,wiersz,kolum);
						
					}
					b[z[x]]=0;
					wiersz[x/y]-=z[x];
					kolum[y-1]-=z[x];
				}
				return a;
			}
			else		//jesli nie jestem w ostatnim elemencie wiersza wyszukuje element, ktory nie zostal uzyty
			{
				for(int g=1;g<=y*y;g++)
				{
					if(b[g]==0)
					{
						b[g]=1;
						wiersz[x/y]+=g;
						kolum[x%y]+=g;
						z[x]=g;
						if(wiersz[x/y]<sum && kolum[x%y]<sum)
						{
							if(x/y==y-2)			//jesli jestem w przedostatnim wierszu uzupelniam ostatni wiersz(jesli moge - suma-(suma na kolumnie))
							{
								if(b[sum-kolum[x%y]]==0)
								{
									b[sum-kolum[x%y]]=1;
									z[x+y]=sum-kolum[x%y];
									a=perm(a,b,x+1,y,z,wiersz,kolum);
									b[sum-kolum[x%y]]=0;
								}
							}
							else
								a=perm(a,b,x+1,y,z,wiersz,kolum);
						}
						else		//jesli element, ktory dodaje przekracza sume w wierszu lun kolumnie oznacza, ze kazdy nastepny tez przekroczy wiec wychodze z warstwy
						{
							b[g]=0;
							wiersz[x/y]-=g;
							kolum[x%y]-=g;
							return a;
						}
						b[g]=0;
						wiersz[x/y]-=g;
						kolum[x%y]-=g;
					}
				}

			}
		}
		else	//jesli wiersz jest nieparzysty to zaczynam uzupelnianie wiersza od konca
		{
			if(x%y==y-1)	//jesli jestem w ostatnim elemencie wiersza moge go wypelnic
			{
				if(b[sum-wiersz[x/y]]==0 && sum-wiersz[x/y]<=y*y)
				{
					z[(x/y)*y+y-x%y-1]=sum-wiersz[x/y];
					b[z[(x/y)*y+y-x%y-1]]=1;
					wiersz[x/y]=sum;
					kolum[0]+=z[(x/y)*y+y-x%y-1];
					if(kolum[0]<sum)
					{
						if(x/y==y-2)	//jesli jestem w przedostatnim wierszu uzupelniam ostatni wiersz(jesli moge - suma-(suma na kolumnie))
						{
							if(b[sum-kolum[0]]==0)
							{
								b[sum-kolum[0]]=1;
								z[(x/y)*y+y-x%y-1+y]=sum-kolum[0];
								a=perm(a,b,x+1,y,z,wiersz,kolum);
								b[sum-kolum[0]]=0;
							}
						}
						else
							a=perm(a,b,x+1,y,z,wiersz,kolum);
					
					}
					b[z[(x/y)*y+y-x%y-1]]=0;
					wiersz[x/y]-=z[(x/y)*y+y-x%y-1];
					kolum[0]-=z[(x/y)*y+y-x%y-1];
				}
				return a;
			}
			else			//jesli nie jestem w ostatnim elemencie wiersza wyszukuje element, ktory nie zostal uzyty
			{
				for(int g=1;g<=y*y;g++)
				{
					if(b[g]==0)
					{
						b[g]=1;
						wiersz[x/y]+=g;
						kolum[y-x%y-1]+=g;
						z[(x/y)*y+y-x%y-1]=g;
						if(wiersz[x/y]<sum && kolum[y-x%y-1]<sum)
						{
							if(x/y==y-2)		//jesli jestem w przedostatnim wierszu uzupelniam ostatni wiersz(jesli moge - suma-(suma na kolumnie))
							{
								if(b[sum-kolum[y-x%y-1]]==0)
								{
									b[sum-kolum[y-x%y-1]]=1;
									z[(x/y)*y+y-x%y-1+y]=sum-kolum[y-x%y-1];
									a=perm(a,b,x+1,y,z,wiersz,kolum);
									b[sum-kolum[y-x%y-1]]=0;
								}
							}
							else
								a=perm(a,b,x+1,y,z,wiersz,kolum);

						}
						else	//jesli element, ktory dodaje przekracza sume w wierszu lun kolumnie oznacza, ze kazdy nastepny tez przekroczy wiec wychodze z warstwy
						{
							b[g]=0;
							wiersz[x/y]-=g;
							kolum[y-x%y-1]-=g;
							return a;
						}
						b[g]=0;
						wiersz[x/y]-=g;
						kolum[y-x%y-1]-=g;
					}
				}
			}
		}
	}
	return a;
}
bool magkwad(int x,int y[])	//sprawdzam czy sumy na przekatnych sa prawidlowe
{
    int suma=0,sumaog=(x*(x*x+1))/2;
    for(int g=1;g<=x;g++)
        suma+=y[(x+1)*(g-1)];
    if(suma!=sumaog)
            return false;

    suma=0;
    for(int g=1;g<=x;g++)
        suma+=y[(x-1)*(g)];
    if(suma!=sumaog)
            return false;
    return true;
}
