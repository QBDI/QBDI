/*
 * This file is part of QBDI.
 *
 * Copyright 2017 Quarkslab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "Platform.h"
#include "Utility/System.h"
#include "llvm/MC/MCObjectWriter.h"

#include "TestSetup/InMemoryAssembler.h"

InMemoryObject::InMemoryObject(const char* source, const char* cpu, const char** mattrs) {
    std::unique_ptr<llvm::MCAsmInfo>         MAI;
    std::unique_ptr<llvm::MCContext>         MCTX;
    std::unique_ptr<llvm::MCInstrInfo>       MCII;
    std::unique_ptr<llvm::MCObjectFileInfo>  MOFI;
    std::unique_ptr<llvm::MCRegisterInfo>    MRI;
    std::unique_ptr<llvm::MCSubtargetInfo>   MSTI;
    std::unique_ptr<llvm::MCStreamer>        mcStr;
    const llvm::Target*                      processTarget;
    llvm::SourceMgr                          SrcMgr;
    std::string                              tripleName;
    std::string                              featuresStr;
    std::error_code                          ec;
    llvm::SmallVector<char, 1024>            objectVector;
 
    if(mattrs != nullptr) {
        llvm::SubtargetFeatures features;
        for (unsigned i = 0; mattrs[i] != nullptr; i++) {
            features.AddFeature(mattrs[i]);
        }
        featuresStr = features.getString();
    }

    std::string error;
    // lookup target
    llvm::Triple processTriple(llvm::sys::getDefaultTargetTriple());
    // make sure we don't accidentally use a weak object file format
    processTriple.setObjectFormat(llvm::Triple::ObjectFormatType::ELF);
    processTriple.setOS(llvm::Triple::OSType::Linux);
    tripleName = llvm::Triple::normalize(processTriple.str());
    processTarget = llvm::TargetRegistry::lookupTarget(tripleName, error);
    // Allocate all LLVM classes
    MRI = std::unique_ptr<llvm::MCRegisterInfo>(
        processTarget->createMCRegInfo(tripleName)
    );
    MAI = std::unique_ptr<llvm::MCAsmInfo>(
        processTarget->createMCAsmInfo(*MRI, tripleName)
    );
    MOFI = std::unique_ptr<llvm::MCObjectFileInfo>(new llvm::MCObjectFileInfo());
    MCTX = std::unique_ptr<llvm::MCContext>(
        new llvm::MCContext(MAI.get(), MRI.get(), MOFI.get(), &SrcMgr)
    );
    MOFI->InitMCObjectFileInfo(processTriple, false, *MCTX);
    MCII = std::unique_ptr<llvm::MCInstrInfo>(processTarget->createMCInstrInfo());
    MSTI = std::unique_ptr<llvm::MCSubtargetInfo>(
      processTarget->createMCSubtargetInfo(tripleName, cpu, featuresStr)
    );
    auto MAB = std::unique_ptr<llvm::MCAsmBackend>(processTarget->createMCAsmBackend(*MSTI, *MRI, llvm::MCTargetOptions()));
    auto MCE = std::unique_ptr<llvm::MCCodeEmitter>(processTarget->createMCCodeEmitter(*MCII, *MRI, *MCTX));
   
    // Wrap output object into raw_ostream
    //raw_pwrite_string_ostream rpsos(objectStr);
    llvm::raw_svector_ostream rsos(objectVector);
    auto objectWriter = std::unique_ptr<llvm::MCObjectWriter>(
        MAB->createObjectWriter(rsos)
     );

    // Add input to the SourceMgr
    SrcMgr.AddNewSourceBuffer(
        llvm::MemoryBuffer::getMemBuffer(llvm::StringRef(source)),
        llvm::SMLoc()
    );
    // Set MCStreamer as a MCObjectStreamer
    mcStr.reset(processTarget->createMCObjectStreamer(MSTI->getTargetTriple(), *MCTX, std::move(MAB), 
                                                      std::move(objectWriter), std::move(MCE), *MSTI, true, false, false));
    // Create the assembly parsers
    llvm::MCAsmParser* parser = llvm::createMCAsmParser(SrcMgr, *MCTX, *mcStr, *MAI);
    llvm::MCTargetAsmParser* tap = 
        processTarget->createMCAsmParser(*MSTI, *parser, *MCII, llvm::MCTargetOptions());
    parser->setTargetParser(*tap);
    // Finally do something we care about
    mcStr->InitSections(false);
    EXPECT_FALSE(parser->Run(true));
    delete parser;
    delete tap;
    // Copy object into new page and make it executable
    unsigned mFlags = PF::MF_READ | PF::MF_WRITE;
#if defined(QBDI_OS_IOS)
    mFlags |= PF::MF_EXEC;
#endif
    objectBlock = QBDI::allocateMappedMemory(objectVector.size(), nullptr, mFlags, ec);
    memcpy(objectBlock.base(), objectVector.data(), objectVector.size());
#if !defined(QBDI_OS_IOS)
    llvm::sys::Memory::protectMappedMemory(objectBlock, PF::MF_READ | PF::MF_EXEC);
#endif

    // LLVM Insanity Oriented Programming
    llvm::Expected<std::unique_ptr<llvm::object::ObjectFile>> obj_ptr = 
        llvm::object::ObjectFile::createObjectFile(
            llvm::MemoryBufferRef(
                llvm::StringRef(
                    (const char*) objectBlock.base(),
                    objectBlock.size()
                ),
                ""
            )
    );
    if ((bool) obj_ptr == false) {
        llvm::errs() << "Failed to load input file" << '\n';
        return;
    }

    // Finding the .text section of the object
    llvm::object::ObjectFile *object = obj_ptr.get().get();
    llvm::StringRef text_section;
    for(auto sit = object->sections().begin(); sit != object->sections().end(); ++sit)
    {
        if(sit->isText()) {
            if((ec = sit->getContents(text_section))) {
                llvm::errs() << "Failed to load text section: " << ec.message() << "\n";
                return;
            }
            else {
                break;
            }
        }
    }
    EXPECT_LT((unsigned int) 0, text_section.size());
    code = llvm::ArrayRef<uint8_t>((const uint8_t*) text_section.data(), text_section.size());
}
