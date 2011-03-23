#include "djFilter.h"

/* Conversion constants */
#define Nhc       8
#define Na        7
#define Np       (Nhc+Na)
#define Npc      (1<<Nhc)
#define Amask    ((1<<Na)-1)
#define Pmask    ((1<<Np)-1)
#define Nh       16
#define Nb       16
#define Nhxn     14
#define Nhg      (Nh-Nhxn)
#define NLpScl   13

#define MAX_HWORD 32767
#define MIN_HWORD -32768

static inline short WordToHword(unsigned short v, int scl)
{
    short out;
    unsigned short llsb = (1<<(scl-1));
    v += llsb;          /* round */
    v >>= scl;
    if (v>MAX_HWORD) {
        v = MAX_HWORD;
    } else if (v < MIN_HWORD) {
         v = MIN_HWORD;
    }   
    out = (short) v;
    return out;
}

static int 
  SrcLinear(short X[], short Y[], double factor, unsigned short *Time, unsigned int Nx)
{
    short iconst;
    short *Xp, *Ystart;
    unsigned short v,x1,x2;
    
    double dt;                  /* Step through input signal */ 
    unsigned short dtb;                  /* Fixed-point version of Dt */
    unsigned short endTime;              /* When Time reaches EndTime, return to user */
    
    dt = 1.0/factor;            /* Output sampling period */
    dtb = dt*(1<<Np) + 0.5;     /* Fixed-point representation */
    
    Ystart = Y;
    endTime = *Time + (1<<Np)*(unsigned short)Nx;
    while (*Time < endTime)
    {
        iconst = (*Time) & Pmask;
        Xp = &X[(*Time)>>Np];      /* Ptr to current input sample */
        x1 = *Xp++;
        x2 = *Xp;
        x1 *= ((1<<Np)-iconst);
        x2 *= iconst;
        v = x1 + x2;
        *Y++ = WordToHword(v,Np);   /* Deposit output */
        *Time += dtb;               /* Move to next sample by time increment */
    }
    return (Y - Ystart);            /* Return number of output samples */
}

Sampler::Sampler()
{
    Y1 = NULL;
    Y2 = NULL;
    OBUFFSIZE = 0;
    m_factor = 0;


}

Sampler::~Sampler();
{
    delete [] Y1;
    delete [] Y2;

}

void * Sampler::ProcessBlock(void * inBlock, double factor, int &numsamples, int samplebits, int channels)
{

    if ((factor != m_factor) || (OBUFFSIZE == 0))
    {
        OBUFFSIZE = (int)(((double)IBUFFSIZE)*factor+2.0);
        delete [] Y1;
        delete [] Y2;
        Y1 = new short[OBUFFSIZE];
        Y2 = new short[OBUFFSIZE];
        m_factor = factor;
    }

    SrcLinear(short X[], short Y[], double factor, unsigned short *Time, unsigned int Nx)
{
    short iconst;
    short *Xp, *Ystart;
    unsigned short v,x1,x2;
    
    double dt;                  /* Step through input signal */ 
    unsigned short dtb;                  /* Fixed-point version of Dt */
    unsigned short endTime;              /* When Time reaches EndTime, return to user */
    
    dt = 1.0/factor;            /* Output sampling period */
    dtb = dt*(1<<Np) + 0.5;     /* Fixed-point representation */
    
    Ystart = Y;
    endTime = *Time + (1<<Np)*(unsigned short)Nx;
    while (*Time < endTime)
    {
        iconst = (*Time) & Pmask;
        Xp = &X[(*Time)>>Np];      /* Ptr to current input sample */
        x1 = *Xp++;
        x2 = *Xp;
        x1 *= ((1<<Np)-iconst);
        x2 *= iconst;
        v = x1 + x2;
        *Y++ = WordToHword(v,Np);   /* Deposit output */
        *Time += dtb;               /* Move to next sample by time increment */
    }
    return (Y - Ystart);            /* Return number of output samples */
}
   

}


#if 0

EQOptions &EQOptions::operator=(const EQOptions &other)
{


    return *this;
}

SamplerOptions &SamplerOptions::operator=(const SamplerOptions &other)
{


    return *this;
}

void Filter::Add(Filter * flt)
{
    if (m_next)
        m_next->Add(flt);
    else 
        m_next = flt;
}

    // returns non-null only if it was head node and new node becomes head
Filter * Filter::Remove(const string& name)
{
    if (m_name == name)
    {
        if (m_next) // if there is one after this
            if (m_prev) // if there was one before this
            {
                m_prev->m_next = m_next; // set prev->next to be next
                m_next->m_prev = m_prev; // set next->prev to be prev
                m_next = NULL; // so we dont delete the whole chain
                delete this; // deallocate
                return NULL;
            }
            else
            {
                Filter * tmp = m_next;
                m_next->m_prev = NULL; // set the next one to head node
                m_next = NULL; // so we dont delete whole chain
                delete this;
                return m_next;
            }
        else
            if (m_prev) // prev but no next
            {
                m_prev->m_next = NULL;
                delete this;
                return NULL;
            }
            else // no prev or next
            {
                delete this;
                return NULL;
            }
    }
    else 
        if (m_next)
            return m_next->Remove(name);
        else
            return NULL;
}

bool Filter::SetOptions(const string& name, FilterOptions * options)
{
    if (m_name == name)
    {
        return DoSetOptions(options);
    }
    else
        if (m_next)
            return m_next->SetOptions(name, options);
        else
            return false;
}

bool Filter::IsOk(const string& name)
{
    if (m_name == name)
    {
        return DoIsOk();
    }
    else
        if (m_next)
            return m_next->IsOk(name);
        else
            return false;

}

bool Filter::Start(const string& name)
{
    if (m_name == name)
    {
        m_stop = false;
        return true;
    }
    else
        if (m_next)
            return m_next->Start(name);
        else
            return false;

}

bool Filter::Stop(const string& name)
{    
    if (m_name == name)
    {
        m_stop = true;
        return true;
    }
    else
        if (m_next)
            return m_next->Stop(name);
        else
            return false;

}

void * Filter::ProcessBlock(void * inBlock, int numsamples, int samplebits)
{    
    if (m_next)
        return m_next->Stop(name);
    else
        return false;

}

Equalizer::Equalizer(const string& name, Filter * next, Filter * prev)
: Filter(name, next, prev)
{
}

Equalizer::~Equalizer()
{
}

bool Equalizer::DoSetOptions(FilterOptions * options)
{
    if (!options)
        return false;

    // a basic run-time check
    if (options->GetName() != "EQOptions")
        return false;

    // since the run-time check passed, this will probably be ok...
    EQOptions * tmp = static_cast<EQOptions *>(options);

    m_options = *tmp;

    return true;

}

bool Equalizer::DoIsOk(const string& name)
{
    return true;
}

void * Equalizer::DoProcessBlock(void * inBlock, int numsamples, int samplebits)
{

}
#endif
