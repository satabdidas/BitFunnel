#include <iostream>     // TODO: Remove this temporary header.

#include "BitFunnel/Exceptions.h"
#include "Shard.h"
#include "Term.h"       // TODO: Remove this temporary include.


namespace BitFunnel
{
    Shard::Shard(IIngestor& ingestor, Id id)
        : m_ingestor(ingestor),
          m_id(id),
          m_slice(new Slice(*this))
    {
        m_activeSlice = m_slice.get();
    }


    void Shard::TemporaryAddPosting(Term const & term, DocIndex index)
    {
        std::cout << "  " << index << ": ";
        term.Print(std::cout);
        std::cout << std::endl;

        m_temporaryFrequencyTable[term]++;
    }


    void Shard::TemporaryPrintFrequencies(std::ostream& out)
    {
        out << "Term frequency table for shard " << m_id << ":" << std::endl;
        for (auto it = m_temporaryFrequencyTable.begin(); it != m_temporaryFrequencyTable.end(); ++it)
        {
            out << "  ";
            it->first.Print(out);
            out << ": "
                << it->second
                << std::endl;
        }
        out << std::endl;
    }


    DocumentHandleInternal Shard::AllocateDocument()
    {
        DocIndex index;
        if (!m_activeSlice->TryAllocateDocument(index))
        {
            throw FatalError("In this temporary code, TryAllocateDocument() should always succeed.");
        }
        return DocumentHandleInternal(m_activeSlice, index);
    }


    IIngestor& Shard::GetIndex() const
    {
        return m_ingestor;
    }
}
