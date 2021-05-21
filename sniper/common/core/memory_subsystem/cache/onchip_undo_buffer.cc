#include "onchip_undo_buffer.h"

OnChipUndoBuffer::OnChipUndoBuffer(UInt16 num_entries) : m_num_entries(num_entries), m_buffer(num_entries)
{
}

OnChipUndoBuffer::~OnChipUndoBuffer()
{
}
