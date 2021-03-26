
#include <QBDI.h>

namespace QBDI {
extern "C" {

QBDI_EXPORT rword qbdi_getGPR(GPRState *state, uint32_t rid) {
  return QBDI_GPR_GET(state, rid);
}

QBDI_EXPORT void qbdi_setGPR(GPRState *state, uint32_t rid, rword val) {
  QBDI_GPR_SET(state, rid, val);
}

struct StructDesc {
  uint32_t size;
  uint32_t items;
  uint32_t offsets[30];
};

QBDI_EXPORT const StructDesc *qbdi_getMemoryAccessStructDesc() {
  static const StructDesc MemoryAccessDesc{
      sizeof(MemoryAccess),
      6,
      {
          offsetof(MemoryAccess, instAddress),
          offsetof(MemoryAccess, accessAddress),
          offsetof(MemoryAccess, value),
          offsetof(MemoryAccess, size),
          offsetof(MemoryAccess, type),
          offsetof(MemoryAccess, flags),
      }};

  return &MemoryAccessDesc;
}

QBDI_EXPORT const StructDesc *qbdi_getVMStateStructDesc() {
  static const StructDesc VMStateDesc{sizeof(VMState),
                                      6,
                                      {
                                          offsetof(VMState, event),
                                          offsetof(VMState, sequenceStart),
                                          offsetof(VMState, sequenceEnd),
                                          offsetof(VMState, basicBlockStart),
                                          offsetof(VMState, basicBlockEnd),
                                          offsetof(VMState, lastSignal),
                                      }};
  return &VMStateDesc;
}

QBDI_EXPORT const StructDesc *qbdi_getOperandAnalysisStructDesc() {
  static const StructDesc OperandAnalysisDesc{
      sizeof(OperandAnalysis),
      8,
      {
          offsetof(OperandAnalysis, type),
          offsetof(OperandAnalysis, flag),
          offsetof(OperandAnalysis, value),
          offsetof(OperandAnalysis, size),
          offsetof(OperandAnalysis, regOff),
          offsetof(OperandAnalysis, regCtxIdx),
          offsetof(OperandAnalysis, regName),
          offsetof(OperandAnalysis, regAccess),
      }};
  return &OperandAnalysisDesc;
}

QBDI_EXPORT const StructDesc *qbdi_getInstAnalysisStructDesc() {
  static const StructDesc InstAnalysisDesc{
      sizeof(InstAnalysis),
      21,
      {
          offsetof(InstAnalysis, mnemonic),
          offsetof(InstAnalysis, disassembly),
          offsetof(InstAnalysis, address),
          offsetof(InstAnalysis, instSize),
          offsetof(InstAnalysis, affectControlFlow),
          offsetof(InstAnalysis, isBranch),
          offsetof(InstAnalysis, isCall),
          offsetof(InstAnalysis, isReturn),
          offsetof(InstAnalysis, isCompare),
          offsetof(InstAnalysis, isPredicable),
          offsetof(InstAnalysis, mayLoad),
          offsetof(InstAnalysis, mayStore),
          offsetof(InstAnalysis, loadSize),
          offsetof(InstAnalysis, storeSize),
          offsetof(InstAnalysis, condition),
          offsetof(InstAnalysis, flagsAccess),
          offsetof(InstAnalysis, numOperands),
          offsetof(InstAnalysis, operands),
          offsetof(InstAnalysis, symbol),
          offsetof(InstAnalysis, symbolOffset),
          offsetof(InstAnalysis, module),
      }};
  return &InstAnalysisDesc;
}

} // extern "C"
} // namespace QBDI
