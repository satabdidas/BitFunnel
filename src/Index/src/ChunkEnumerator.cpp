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

#include "BitFunnel/Index/IChunkManifestIngestor.h"
#include "BitFunnel/Utilities/ITaskDistributor.h"
#include "BitFunnel/Utilities/Factories.h"
#include "ChunkEnumerator.h"
#include "ChunkTaskProcessor.h"


namespace BitFunnel
{
    ChunkEnumerator::ChunkEnumerator(
        IChunkManifestIngestor const & manifest,
        size_t threadCount)
    {
        std::vector<std::unique_ptr<ITaskProcessor>> processors;
        for (size_t i = 0; i < threadCount; ++i) {
            processors.push_back(
                std::unique_ptr<ITaskProcessor>(
                    new ChunkTaskProcessor(manifest)));
        }

        if (threadCount > 1)
        {
            m_distributor = Factories::CreateTaskDistributor(processors, manifest.GetChunkCount());
        }
        else
        {
            // The threadCount == 1 case is implemented to simplify debugging.
            for (size_t i = 0; i < manifest.GetChunkCount(); ++i) {
                processors[0]->ProcessTask(i);
            }
        }
    }


    void ChunkEnumerator::WaitForCompletion() const
    {
        if (m_distributor != nullptr)
        {
            m_distributor->WaitForCompletion();
        }
    }
}
