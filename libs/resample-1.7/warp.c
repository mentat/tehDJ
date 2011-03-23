/* warp.c - dynamic rate resampling program */

/*
 * Implement dynamic sampling-rate changes; uses a second file to
 * indicate where periods should fall.  This program may be used to add
 * or remove vibrato and micro pitch variations from a sound sample.
 */

#include "filterkit.h"
#include "resample.h"

#include <libc.h>
#include <math.h>

#define MAXNWING   8192
#define MAXFACTOR  4

#define IBUFFSIZE 1024                         /* Input buffer size */
#define OBUFFSIZE (IBUFFSIZE*MAXFACTOR+2)      /* Calc'd out buffer size */


static void fail(char *s)
{
   printf("warp: %s\n\n", s);     /* Display error message  */
   exit(1);                       /* Exit, indicating error */
}

static void fails(char *s, char *s2)
{
   printf("warp: ");              /* Display error message  */
   printf(s, s2);
   printf("\n\n");
   exit(1);                       /* Exit, indicating error */
}

/* Global file pointers: */
FILE *fin, *fout;


static void openData(int argc, char *argv[])
{
   if (argc != 3)
      fail("format is 'warp <file-in> <file-out>'");
   if (NULL == (fin = fopen(*++argv,"r")))
      fails("could not open <file-in> file '%s'",*argv);
   if (NULL == (fout = fopen(*++argv,"w")))
      fails("could not open <file-out> file '%s'",*argv);
}


static void closeData(void)
{
   (void) fclose(fin);
   (void) fclose(fout);
}

static int readData(HWORD Data[], int DataArraySize, int Xoff)
     /* return: 0 - notDone */
     /*        >0 - index of last sample */
{
   int Nsamps, Scan, val;
   HWORD *DataStart;

   DataStart = Data;
   Nsamps = DataArraySize - Xoff;   /* Calculate number of samples to get */
   Data += Xoff;                    /* Start at designated sample number */
   for (; Nsamps>0; Nsamps--)
      {
      Scan = fscanf(fin, "%d", &val);      /* Read in Nsamps samples */
      if (Scan==EOF || Scan==0)            /*   unless read error or EOF */
         break;                            /*   in which case we exit and */
      *Data++ = val;
      }
   if (Nsamps > 0)
      {
      val = Data - DataStart;              /* (Calc return value) */
      while (--Nsamps >= 0)                /*   fill unread spaces with 0's */
         *Data++ = 0;                      /*   and return FALSE */
      return(val);
      }
   return(0);
}



static void writeData(int Nout, HWORD Data[])
{
   while (--Nout >= 0)                  /* Write Nout samples to ascii file */
      fprintf(fout, "%d\n", *Data++);
}


void check(void)
{
   /* Check for illegal constants */
   if (Np >= 16)
      fail("Error: Np>=16");
   if (Nb+Nhg+NLpScl >= 32)
      fail("Error: Nb+Nhg+NLpScl>=32");
   if (Nh+Nb > 32)
      fail("Error: Nh+Nb>32");
}

/* Globals for warping frequency */
double a,b,c,d,e,f,Total;
int Acc;

void initWarp(void)
{
   Total = GetDouble("\n# of input samples",12000.0,"");

   printf("Warping function:  Fout/Fin = w(n)\n");
   printf("  w(n) = off + [amp * sin(ph1+frq1*n/N) * sin(ph2+frq2*n/N)]\n");
   printf("  where: off,amp,ph1   - parameters\n");
   printf("         frq1,ph2,frq2 - more parameters\n");
   printf("         n             - current input sample number\n");
   printf("         N             - total number of input samples\n");

   a = GetDouble("off",1.5,"");
   b = GetDouble("amp",-0.5,"");
   c = D2R * GetDouble("ph1 (degrees)",-90.0,"");
   d = GetDouble("frq1",1.0,"");
   e = D2R * GetDouble("ph2 (degrees)",90.0,"");
   f = GetDouble("frq2",0.0,"");
}


double warpFunction(UWORD Time)
{
   double fTime,t;

   /* Calc absolute position in input file: */
   fTime = (double)Time / (double)(1<<Np) + (double)Acc;
   /* Calc fraction of file processed: */
   t = fTime/Total;
   /* Return warp factor: */
   return (1.0 / (a + b*sin(c+PI2*d*t)*sin(e+PI2*f*t)));
}


/* Sampling rate conversion subroutine */

static int SrcUD(HWORD X[], HWORD Y[], UWORD *Time,
		 UHWORD Nx, UHWORD Nwing, UHWORD LpScl,
		 HWORD Imp[], HWORD ImpD[], BOOL Interp)
{
   HWORD *Xp, *Ystart;
   WORD v;

   UHWORD NewScl;              /* Unity gain scale factor */
   double dh;                  /* Step through filter impulse response */
   double dt;                  /* Step through input signal */
   UWORD endTime;              /* When Time reaches EndTime, return to user */
   UWORD dhb, dtb;             /* Fixed-point versions of Dh,Dt */
   double Factor;              /* Current resampling factor */

   Ystart = Y;
   endTime = *Time + (1<<Np)*(WORD)Nx;
   while (*Time < endTime)
      {
      Factor = warpFunction(*Time);     /* Get new conversion Factor */
      NewScl = LpScl * Factor;          /* Calc new normalizing scalar */
      dt = 1.0 / Factor;                /* New output sampling period */
      dtb = dt*(1<<Np) + 0.5;           /* Fixed-point representation */
      dh = MIN(Npc, Factor * Npc);      /* New filter sampling period */
      dhb = dh*(1<<Na) + 0.5;           /* Fixed-point representation */
      Xp = &X[*Time>>Np];               /* Ptr to current input sample */
      v = FilterUD(Imp, ImpD, Nwing, Interp, Xp, (HWORD)(*Time&Pmask),
          -1, dhb);                     /* Perform left-wing inner product */
      v += FilterUD(Imp, ImpD, Nwing, Interp, Xp+1, (HWORD)((-*Time)&Pmask),
           1, dhb);                     /* Perform right-wing inner product */
      v >>= Nhg;                        /* Make guard bits */
      v *= NewScl;                      /* Normalize for unity filter gain */
      *Y++ = v>>NLpScl;                 /* Deposit output */
      *Time += dtb;                     /* Move to next sample by time inc */
      }
   return (Y - Ystart);                 /* Return the # of output samples */
}

static int localResample(HWORD Imp[], HWORD ImpD[],
		    UHWORD LpScl, UHWORD Nmult, UHWORD Nwing,
		    BOOL InterpFilt)
{
   UWORD Time;                       /* Current time/pos in input sample */
   UHWORD Xp, Xread, Ncreep, Xoff;
   HWORD X[IBUFFSIZE], Y[OBUFFSIZE]; /* I/O buffers */
   UHWORD Nout, Nx;
   int i, Ycount, last;

   /* Calc reach of LP filter wing & give some creeping room */
   Xoff = ((Nmult+1)/2.0) * MAX(1.0,1.0*MAXFACTOR) + 10;
   if (IBUFFSIZE < 2*Xoff)      /* Check input buffer size */
      fail("IBUFFSIZE (or Factor) is too small");
   Nx = IBUFFSIZE - 2*Xoff;     /* # of samples to proccess each iteration */

   last = FALSE;                /* Have we read last input sample yet? */
   Ycount = 0;                  /* Output sample # and length of out file */
   Xp = Xoff;                   /* Current position in input buffer */
   Xread = Xoff;                /* Position in input array to read into */
   Time = (Xoff<<Np);           /* Current-time pointer for converter */
   Acc = -Xoff;                 /* Acc + int(Time) = index into input file */

   for (i=0; i<Xoff; X[i++]=0); /* Need Xoff zeros at begining of sample */

   do {
      if (!last)                /* If haven't read last sample yet */
         {
         last = readData(X, IBUFFSIZE, (int)Xread); /* Fill input buffer */
         if (last && (last+Xoff<Nx))  /* If last sample has been read... */
            Nx = last + Xoff;   /* ...calc last sample affected by filter */
	 }
      Ycount += Nout = SrcUD(X,Y,&Time,Nx,Nwing,LpScl,Imp,ImpD,InterpFilt);
      Time -= (Nx<<Np);           /* Move converter Nx samples back in time */
      Xp += Nx;                   /* Advance by number of samples processed */
      Acc += Nx;                  /* We moved Nx samples in the input file */
      Ncreep = (Time>>Np) - Xoff; /* Calc time accumulation in Time */
      if (Ncreep)
         {
         Time -= (Ncreep<<Np);    /* Remove time accumulation */
         Xp += Ncreep;            /* and add it to read pointer */
         Acc += Ncreep;
         }
      for (i=0; i<IBUFFSIZE-Xp+Xoff; i++)   /* Copy portion of input signal */
         X[i] = X[i+Xp-Xoff];               /*    that must be re-used */
      if (last)                             /* If near end of sample... */
         {
         last -= Xp;             /* ...keep track were it ends */
         if (!last)              /* Lengthen input by 1 sample */
            last++;              /*  if needed to keep flag TRUE */
	 }
      Xread = i;                 /* Pos in input buff to read new data into */
      Xp = Xoff;

      if (Nout > OBUFFSIZE)      /* Check to see if output buff overflowed */
         fail("Output array overflow");

      writeData((int)Nout, Y);   /* Write data in output buff to file */
      } while (last >= 0);       /* Continue until done processing samples */
   return(Ycount);               /* Return # of samples in output file */
}


static HWORD Imp[MAXNWING];         /* Filter coefficients */
static HWORD ImpD[MAXNWING];        /* ImpD[n] = Imp[n+1]-Imp[n] */

void main(int argc, char *argv[])
{
   BOOL InterpFilt;             /* TRUE means interpolate filter coeffs */
   UHWORD LpScl, Nmult, Nwing;
   int outCount;

   Nmult = 13;
   InterpFilt = TRUE;
   check();                      /* Check constants for validity */
   openData(argc, argv);         /* Interp arguments and open i&o files */
   initWarp();                   /* Set up the warp function's parameters */
   if (readFilter(NULL, (HWORD **)&Imp, (HWORD **)&ImpD, &LpScl, &Nmult, &Nwing))
      fail("could not open Filter file, or syntax error in filter file");
   printf("\nConverting . . . ");
   outCount = localResample(Imp, ImpD, LpScl, Nmult, Nwing, InterpFilt);
   closeData();
   printf("Done:  %d output samples.\n\n", outCount);
}
