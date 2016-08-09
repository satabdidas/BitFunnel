// The MIT License (MIT)

// Copyright (c) 2016, Microsoft

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <sstream>

#include "BitFunnel/BitFunnelTypes.h"
#include "BitFunnel/Exceptions.h"
#include "BitFunnel/Term.h"
#include "TermTable.h"



namespace BitFunnel
{
    //*************************************************************************
    //
    // TermTable
    //
    //*************************************************************************
    void TermTable::OpenTerm()
    {
        m_start = static_cast<RowIndex>(m_rowIds.size());
    }


    void TermTable::AddRowId(RowId row)
    {
        m_rowIds.push_back(row);
    }


    // TODO: PackedRowIdSequence::Type parameter.
    void TermTable::CloseTerm(Term::Hash hash)
    {
        // Verify that this Term::Hash hasn't been added previously.
        auto it = m_termHashToRows.find(hash);
        if (it != m_termHashToRows.end())
        {
            std::stringstream message;
            message << "TermTable::CloseTerm(): Term::Hash " << hash << " has already been added.";

            RecoverableError error(message.str());
        }

        RowIndex end = static_cast<RowIndex>(m_rowIds.size());
        m_termHashToRows.insert(
            std::make_pair(hash,
                           PackedRowIdSequence(
                               m_start,
                               end,
                               PackedRowIdSequence::Type::Explicit)));
    }


    PackedRowIdSequence TermTable::GetRows(const Term& term) const
    {
        // TODO: Implement adhoc.
        // TODO: Implement facts.

        auto it = m_termHashToRows.find(term.GetRawHash());
        if (it != m_termHashToRows.end())
        {
            // For now, treat all terms as explicit.
            return (*it).second;
        }
        else
        {
            // For now, if term isn't found, just return and empty row sequence.
            return PackedRowIdSequence(0, 0, PackedRowIdSequence::Type::Adhoc);
        }
    }


    RowId TermTable::GetRowId(size_t rowOffset) const
    {
        // TODO: Error checking?
        return m_rowIds[rowOffset];
    }



    //size_t TermTable::GetTotalRowCount(Rank /*rank*/) const
    //{
    //    throw 0;
    //}

    //TermTable::const_iterator TermTable::GetRows(Term term) const
    //{
    //    auto it = m_termHashToRows.find(term.GetRawHash());
    //    if (it != m_termHashToRows.end())
    //    {
    //        RowId const * buffer = &m_rowIds[0];
    //        return const_iterator(buffer + (*it).second.first,
    //                              buffer + (*it).second.second);
    //    }
    //    else
    //    {
    //        return end();
    //    }
    //}


    //TermTable::const_iterator TermTable::end() const
    //{
    //    return const_iterator(nullptr, nullptr);
    //}


    //*************************************************************************
    //
    // TermTable::const_iterator
    //
    //*************************************************************************
    //TermTable::const_iterator::const_iterator(RowId const * start, RowId const * end)
    //    : m_current(start),
    //      m_end(end)
    //{
    //}


    //RowId TermTable::const_iterator::operator*() const
    //{
    //    if (m_current == m_end)
    //    {
    //        RecoverableError error("TermTable::const_iterator::operator*: no more entries.");
    //        throw error;
    //    }

    //    return *m_current;
    //}

    //TermTable::const_iterator& TermTable::const_iterator::operator++()
    //{
    //    if (m_current == m_end)
    //    {
    //        RecoverableError error("TermTable::const_iterator::operator++: no more entries.");
    //        throw error;
    //    }

    //    ++m_current;
    //    return *this;
    //}


    //bool TermTable::const_iterator::operator!=(const_iterator const & other) const
    //{
    //    return (m_current != other.m_current) && (m_end != other.m_end);
    //}
}
