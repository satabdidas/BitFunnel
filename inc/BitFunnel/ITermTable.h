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


#pragma once

#include <ostream>

#include "BitFunnel/BitFunnelTypes.h"   // Rank parameter.
#include "BitFunnel/IInterface.h"       // Base class.
#include "ITermTreatment.h"             // RowConfiguration::Entry::c_maxRowCount.
#include "BitFunnel/RowId.h"            // RowId parameter.
#include "BitFunnel/Term.h"             // Term::Hash parameter.


namespace BitFunnel
{
    class PackedTermInfo;

    //class ITermTable2 : public IInterface
    //{
    //public:
    //    virtual void OpenTerm() = 0;

    //    // Adds a single RowId to the term table's RowId buffer.
    //    virtual void AddRowId(RowId id) = 0;

    //    virtual void CloseTerm(Term::Hash term) = 0;

    //    // Returns the total number of rows (private + shared) associated with
    //    // the row table for (rank). This includes rows allocated for
    //    // facts, if applicable.
    //    //virtual size_t GetTotalRowCount(Rank rank) const = 0;

    //    // Returns a PackedRowIdSequence structure associated with the
    //    // specified term. The PackedRowIdSequence structure contains
    //    // information about the term's rows. PackedRowIdSequence is used
    //    // by RowIdSequence to implement RowId enumeration for regular, adhoc
    //    // and fact terms.
    //    class PackedRowIdSequence;
    //    virtual PackedRowIdSequence GetTermInfo(const Term& term) const = 0;


    //    class PackedRowIdSequence
    //    {
    //    public:
    //        enum class Type
    //        {
    //            Explicit = 0,
    //            Adhoc = 1,
    //            Fact = 2,
    //            Last = Fact
    //            // WARNING: update c_log2MaxTypeValue, c_maxTypeValue when
    //            // adding new enumeration values to Types.
    //        };
    //        static const size_t c_log2MaxTypeValue = 2;
    //        static const Type c_maxTypeValue = Type::Last;

    //        PackedRowIdSequence(RowIndex start,
    //                            RowIndex end,
    //                            Type type);

    //        RowIndex GetStart() const
    //        {
    //            return m_start;
    //        }

    //        RowIndex GetEnd() const
    //        {
    //            return m_start + m_count;
    //        }

    //        Type GetType() const
    //        {
    //            return static_cast<Type>(m_type);
    //        }

    //    private:
    //        const uint32_t m_start : c_log2MaxRowIndexValue;
    //        const uint32_t m_count : RowConfiguration::Entry::c_log2MaxRowCount;
    //        const uint32_t m_type : c_log2MaxTypeValue;
    //    };

    //    // DESIGN NOTE: PackedRowIdSequence is intended to be a small value
    //    // type. Considerations include:
    //    //     Term table may potentially store millions of PackedRowIdSequence.
    //    //         ==> Small data structure.
    //    //     PackedRowIdSequence returned for each term in query
    //    //         ==> No memory allocations for copy.
    //    //         ==> Value type.
    //    static_assert(sizeof(PackedRowIdSequence) == sizeof(uint32_t), "PackedRowIDSequence too large.");
    //};


    //*************************************************************************
    //
    // ITermTable is an abstract base class or interface for classes providing
    // a mapping from Term to PackedTermInfo. The PackedTermInfo represents
    // a collection of RowId which can be accessed via the TermInfo class.
    //
    // TODO: ITermTable should probably be factored into two separate
    // interfaces - one for build time and one for serve time.
    // TFS 15152.
    //
    //*************************************************************************
    class ITermTable
    {
    public:
        virtual ~ITermTable() {}

        // Writes the TermTable to a stream.
        virtual void Write(std::ostream& stream) const = 0;

        // Adds a single RowId to the term table's RowId buffer.
        virtual void AddRowId(RowId id) = 0;

        // Returns the total number of RowIds currently in the table.
        // Typically used to get record a term's first RowId offset before
        // adding its rows.
        virtual uint32_t GetRowIdCount() const = 0;

        // Returns the RowId at the specified offset in the TermTable.
        virtual RowId GetRowId(size_t rowOffset) const = 0;

        // Returns a RowId for an adhoc term. The resulting RowId takes its
        // shard and tier from the RowId at the specified rowOffset. Its
        // RowIndex is based on a combination of the hash, the variant, and the
        // RowIndex in the RowId at the specified rowOffset.
        //
        // TODO: is the variant parameter really necessary? Isn't it sufficient
        // for the TermTableBuilder to create a set of adhoc RowIds that all
        // have different RowIndex values?
        virtual RowId GetRowIdAdhoc(Term::Hash hash, size_t rowOffset, size_t variant)  const = 0;

        // Returns a RowId for a fact term. rowOffset specifies an index of the
        // fact in the list of configured facts.
        virtual RowId GetRowIdForFact(size_t rowOffset) const = 0;

        // Adds a term to the term table. The term's rows must be added first
        // by calling AddRowId().
        virtual void AddTerm(Term::Hash hash, size_t rowIdOffset, size_t rowIdLength) = 0;

        // Adds an adhoc term to the term table. The design intent is that this
        // method will be called during term table build to supply the set of
        // RowIds that will be used as a pattern for adhoc terms.
        /*
        virtual void AddTermAdhoc(Stream::Classification classification,
                                  size_t gramSize,
                                  Tier tierHint,
                                  IdfSumX10 idfSum,
                                  size_t rowIdOffset,
                                  size_t rowIdLength) = 0;
        */

        // Specifies the total number of rows and the number of shared rows in
        // the row table associated with the specified (tier, rank)
        // values.
        virtual void SetRowTableSize(Rank rank,
                                     size_t rowCount,
                                     size_t sharedRowCount) = 0;

        // Returns the total number of rows (private + shared) associated with
        // the row table for (rank). This includes rows allocated for
        // facts, if applicable.
        virtual size_t GetTotalRowCount(Rank rank) const = 0;

        // Returns the number of rows associated with the mutable facts for
        // RowTables with (tier, rank).
        virtual size_t GetMutableFactRowCount(Rank rank) const = 0;

        // Enumeration to represent the type of the term - explicitly stored
        // term, adhoc term or a term which represents a specific fact.
        enum TermKind
        {
            Adhoc,
            Disposed,
            Explicit,
            Fact
        };

        // Returns a PackedTermInfo structure associated with the specified
        // term. The PackedTermInfo structure contains information about the
        // term's rows. PackedTermInfo is used by TermInfo to implement RowId
        // enumeration for regular, adhoc and fact terms.
        virtual PackedTermInfo GetTermInfo(const Term& term, TermKind& termKind) const = 0;

        // Creates a term which is used to mark documents as soft-deleted.
        static Term GetSoftDeletedTerm();

        // Creates a term which is used for padding in the row plan when there
        // are not sufficient or no rows at a particular rank. This term
        // corresponds to a single row at DDR rank 0 which has all bits set to 1.
        static Term GetMatchAllTerm();

        // Creates a term which is used for padding in the row plan, or when
        // handling the requests for rows in tiers which are not supported.
        // This term corresponds to a single row at DDR rank 0 which has all
        // bits set to 0.
        static Term GetMatchNoneTerm();
    };
}
