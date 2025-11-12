#ifndef INERTIA_BINARY_HPP
#define INERTIA_BINARY_HPP

/**
 * @file inr/Support/Binary.hpp
 * @brief Inertia's binary support classes.
 *
 * This header contains helpers relating to binary, like for file formats and whatnot.
 * Some stuff are taken from other projects such as jwelf: https://github.com/iDontHaveTime/jwelf
 *
 **/

#include "inr/Defines/Attribute.hpp"
#include "inr/Support/Byte.hpp"
#include "inr/Support/Endian.hpp"
#include "inr/Target/Triple.hpp"

#include <cstdint>
#include <type_traits>

namespace inr{

    /**
     * @brief A generic binary header. Could be used for ELF, PE, Mach-O, etc..
     */
    class binary_header{
    public:
        constexpr binary_header() noexcept = default;
    };

    /* ELF FILES. */

    /**
     * @brief ELF Class type, either 64 or 32bit.
     */
    enum class elf_header_class : uint8_t{
        ELF_32bit = 1, /**< 32bit ELF file. */ 
        ELF_64bit = 2 /**< 64bit ELF file. */
    };
    /**
     * @brief ELF Endian type, little or big. Use 'inr::endian' instead.
     */
    enum class elf_endian_type : uint8_t{
        ELF_Little = 1, /**< Little endian ELF file. */ 
        ELF_Big = 2 /**< Big endian ELF file. */
    };

    /**
     * @brief ELF OS ABI types.
     */
    enum class elf_os_abi : uint8_t{
        ELF_SystemV = 0x00, /**< The SystemV ABI. */
        ELF_HP_UX = 0x01, /**< The HP-UX ABI. */
        ELF_NetBSD = 0x02, /**< The NetBSD ABI. */
        ELF_Linux = 0x03, /**< The Linux ABI. Use SystemV instead. Unless needed */
        ELF_GNU_Hurd = 0x04, /**< The GNU Hurd ABI. */
        ELF_Solaris = 0x06, /**< The Solaris ABI. */
        ELF_AIX = 0x07, /**< The AIX (Monterey) ABI. */
        ELF_IRIX = 0x08, /**< The IRIX ABI. */
        ELF_FreeBSD = 0x09, /**< The FreeBSD ABI. */
        ELF_Tru64 = 0x0A, /**< The Tru64 ABI. */
        ELF_Novell_Modesto = 0x0B, /**< The Novell Modesto ABI. */
        ELF_OpenBSD = 0x0C, /**< The OpenBSD ABI. */
        ELF_OpenVMS = 0x0D, /**< The OpenVMS ABI. */
        ELF_NonStop_Kernel = 0x0E, /**< The NonStop Kernel ABI. */
        ELF_AROS = 0x0F, /**< The AROS ABI. */
        ELF_FenixOS = 0x10, /**< The FenixOS ABI. */
        ELF_Nuxi_CloudABI = 0x11, /**< The Nuxi CloudABI ABI. */
        ELF_Stratus_Technologies_OpenVOS = 0x12, /**< The Stratus Technologies OpenVOS ABI. */
        ELF_Standalone = 0xFF /**< No mainstream ABI. */
    };

    /**
     * @brief Types of the ELF file.
     */
    enum class elf_type : uint16_t{
        ELF_NONE = 0x00, /**< Unknown ELF Type. */
        ELF_REL = 0x01, /**< Relocatable ELF Type. */
        ELF_EXEC = 0x02, /**< Executable ELF Type. */
        ELF_DYN = 0x03, /**< Shared ELF Type. */
        ELF_CORE = 0x04, /**< Core ELF Type. */
        ELF_LOOS = 0xFE00, /**< OS Specific. */
        ELF_HIOS = 0xFEFF, /**< OS Specific. */
        ELF_LOPROC = 0xFF00, /**< Processor Specific. */
        ELF_HIPROC = 0xFFFF, /**< Processor Specific. */
    };

    /**
     * @brief The architecture of the ELF.
     */
    enum class elf_isa : uint16_t{
        ELF_NONE = 0x00, /**< No specific ISA. */
        ELF_ATNT_WE_32100 = 0x01, /**< AT&T WE 32100. */
        ELF_SPARC = 0x02, /**< SPARC. */
        ELF_x86 = 0x03, /**< x86. */
        ELF_Motorola_68000 = 0x04, /**< Motorola 68000 (M68k). */
        ELF_Motorola_88000 = 0x05, /**< Motorola 88000 (M88k). */
        ELF_Intel_MCU = 0x06, /**< Intel MCU. */
        ELF_Intel_80860 = 0x07, /**< Intel 80860. */
        ELF_MIPS = 0x08, /**< MIPS. */
        ELF_IBM_System_370 = 0x09, /**< IBM System/370. */
        ELF_MIPS_RS3000_Little_Endian = 0x0A, /**< MIPS RS3000 Little-endian. */
        /* 0x0B - 0x0E Reserved. */
        ELF_Hewlett_Packard_PA_RISC = 0x0F, /**< Hewlett-Packard PA-RISC. */
        ELF_Intel_80960 = 0x13, /**< Intel 80960. */
        ELF_PowerPC32 = 0x14, /**< Power PC. */
        ELF_PowerPC64 = 0x15, /**< Power PC 64bit. */
        ELF_S390 = 0x16, /**< S390 (and S390x). */
        ELF_IBM_SPU_SPC = 0x17, /**< IBM SPU/SPC. */
        /* 0x18 - 0x23 Reserved. */
        ELF_NEC_V800 = 0x24, /**< NEC V800. */
        ELF_Fujitsu_FR20 = 0x25, /**< Fujitsu FR20. */
        ELF_TRW_RH_32 = 0x26, /**< TRW RH-32. */
        ELF_Motorola_RCE = 0x27, /**< Motorola RCE. */
        ELF_Arm = 0x28, /**< Until Armv7/AArch32. */
        ELF_Digital_Alpha = 0x29, /**< Digital Alpha. */
        ELF_SuperH = 0x2A, /**< SuperH. */
        ELF_SPARC_Version_9 = 0x2B, /**< SPARC Version 9. */
        ELF_Siemens_TriCore = 0x2C, /**< Siemens TriCore. */
        ELF_Argonaut_RISC_Core = 0x2D, /**< Argonaut RISC Core. */
        ELF_Hitachi_H8_300 = 0x2E, /**< Hitachi H8/300. */
        ELF_Hitachi_H8_300H = 0x2F, /**< Hitachi H8/300H. */
        ELF_Hitachi_H8S = 0x30, /**< Hitachi H8S. */
        ELF_Hitachi_H8_500 = 0x31, /**< Hitachi H8/500. */
        ELF_IA_64 = 0x32, /**< IA-64 (not x86-64). */
        ELF_Stanford_MIPS_X = 0x33, /**< Stanford MIPS-X. */
        ELF_Motorola_ColdFire = 0x34, /**< Motorola ColdFire. */
        ELF_Motorola_M68HC12 = 0x35, /**< Motorola M68HC12. */
        ELF_Fujitsu_MMA_Multimedia_Accelerator = 0x36, /**< Fujitsu MMA Multimedia Accelerator. */
        ELF_Siemens_PCP = 0x37, /**< Siemens PCP. */
        ELF_Sony_nCPU_eRISC = 0x38, /**< Sony nCPU embedded RISC Processor. */
        ELF_Denso_NDR1_Microprocessor = 0x39, /**< Denso NDR1 Microprocessor. */
        ELF_Motorola_StarCore_Processor = 0x3A, /**< Motorola Star*Core Processor. */
        ELF_Toyota_ME16_Processor = 0x3B, /**< Toyota ME16 Processor. */
        ELF_STMicroelectronics_ST100_Processor = 0x3C, /**< STMicroelectronics ST100. */
        ELF_Advanced_Logic_Corp_TinyJ = 0x3D, /**< Advanced Logic Corp. TinyJ embedded.*/
        ELF_AMD_x86_64 = 0x3E, /**< Our beloved x86-64, aka AMD64. */
        ELF_x86_64 = ELF_AMD_x86_64, /**< x86-64. */
        ELF_AMD64 = ELF_AMD_x86_64, /**< AMD64. */
        ELF_Sony_DSP_Processor = 0x3F, /**< Sony DSP Processor. */
        ELF_Digital_Equipment_Corp_PDP_10 = 0x40, /**< Digital Equipment Corp. PDP-10. */
        ELF_Digital_Equipment_Corp_PDP_11 = 0x41, /**< Digital Equipment Corp. PDP-11. */
        ELF_Siemens_FX66_Microcontroller = 0x42, /**< Siemens FX66 Microcontroller. */
        ELF_STMicroelectronics_ST9_PLUS = 0x43, /**< STMicroelectronics ST9+ 8/16bit. */
        ELF_STMicroelectronics_ST7 = 0x44, /**< STMicroelectronics ST7 8bit Microcontroller. */
        ELF_Motorola_MC68HC16 = 0x45, /**< Motorola MC68HC16 Microcontroller. */
        ELF_Motorola_MC68HC11 = 0x46, /**< Motorola MC68HC11 Microcontroller. */
        ELF_Motorola_MC68HC08 = 0x47, /**< Motorola MC68HC08 Microcontroller. */
        ELF_Motorola_MC68HC05 = 0x48, /**< Motorola MC68HC05 Microcontroller. */
        ELF_Silicon_Graphics_SVx = 0x49, /**< Silicon Graphics SVx. */
        ELF_STMicroelectronics_ST19 = 0x4A, /**< STMicroelectronics ST19 8bit. */
        ELF_Digital_VAX = 0x4B, /**< Digital VAX. */
        ELF_Axis_Communications_32bit = 0x4C, /**< Axis Communications 32bit embedded. */
        ELF_Infineon_Technologies_32bit = 0x4D, /**< Infineon Technologies 32bit embedded. */
        ELF_Element_14_64bit_DSP = 0x4E, /**< Element 14 64bit DSP Processor. */
        ELF_LSI_Logic_16bit_DSP = 0x4F, /**< LSI Logic 16bit DSP Processor. */
        ELF_TMS320C6000 = 0x8C, /**< TMS320C6000 Family. */
        ELF_MCST_Elbrus_e2k = 0xAF, /**< MCST Elbrus e2k. */
        ELF_Arm64 = 0xB7, /**< Arm 64bit (Armv8/AArch64). */
        ELF_Zilog_Z80 = 0xDC, /**< Zilog Z80. */
        ELF_RISCV = 0xF3, /**< RISC-V. */
        ELF_Berkeley_Packet_Filter = 0xF7, /**< Berkeley Packet Filter. */
        ELF_WDC_65C816 = 0x101, /**< WDC 65C816. */
        ELF_LoongArch = 0x102 /**< LoongArch. */
    };

    template<elf_header_class elf_class>
    class _inr_packed_ elf_program_header_base_class;
    template<elf_header_class elf_class>
    class _inr_packed_ elf_section_header_base_class;

    /**
     * @brief Converts 'inr::endian' to the correct ELF endian type.
     *
     * @param _endian Inertia endian.
     *
     * @return ELF endian.
     */
    constexpr elf_endian_type inr_endian_to_elf(endian _endian) noexcept{
        switch(_endian){
            case endian::little:
                return elf_endian_type::ELF_Little;
            case endian::big:
                return elf_endian_type::ELF_Big;
            default:
                return (elf_endian_type)0;
        }
    }

    /**
     * @brief the e_ident of the ELF.
     */
    class _inr_packed_ elf_ident : public binary_header {
    public:
        using elf_byte = inr::byte;
        struct ident{
            /**
            * @brief ELF Magic, should be 0x7f, E, L, F.
            */
            const elf_byte ei_magic[4] = {'\x7f', 'E', 'L', 'F'};
            /**
            * @brief ELF Class, 32 or 64bit.
            */
            const elf_header_class ei_class;
            /**
            * @brief ELF Endianness, little or big endians.
            */
            const elf_endian_type ei_data;
            /**
            * @brief ELF Version, 1 for the current ELF version.
            */
            const elf_byte ei_version;
            /**
            * @brief ELF ABI, nothing interesting here.
            */
            const elf_os_abi ei_osabi;
            /**
            * @brief ELF ABI Version, most of the time just padding.
            */
            const elf_byte ei_abiversion;
            /**
            * @brief ELF Padding, nothing to see here.
            */
            const elf_byte ei_pad[7] = {0};

            constexpr ident(elf_header_class elf_class, endian elf_endian, elf_os_abi abi, 
                            elf_byte elf_version = 1, elf_byte abi_version = 0) noexcept : 
            ei_class(elf_class), ei_data(inr_endian_to_elf(elf_endian)),
            ei_version(elf_version), ei_osabi(abi), ei_abiversion(abi_version){};
        } e_ident;

        /**
         * @brief Basic constexpr constructor for the ELF.
         *
         * @param elf_class The class of the ELF file.
         * @param elf_endian The endian the ELF will use.
         * @param abi The ABI the ELF uses.
         * @param elf_version The version of the ELF, use 1 unless needed otherwise.
         * @param abi_version The version of the ABI, most ELF interpreters have no definition of this.
         */
        constexpr elf_ident(elf_header_class elf_class, endian elf_endian, elf_os_abi abi, 
                            elf_byte elf_version = 1, elf_byte abi_version = 0) noexcept : 
            e_ident(elf_class, elf_endian, abi, elf_version, abi_version){};
    };

    class _inr_packed_ elf_header : public elf_ident{
        using elf_ident::elf_ident;
    };

    constexpr elf_isa triple_to_elf_isa(const Triple& t) noexcept{
        switch(t.get_arch()){
            case Triple::x86_64:
                return elf_isa::ELF_x86_64;
            case Triple::AArch64:
                return elf_isa::ELF_Arm64;
            default:
                return elf_isa::ELF_NONE;
        }
    }

    constexpr elf_os_abi triple_to_elf_abi(const Triple& t) noexcept{
        switch(t.get_os()){
            case Triple::linux_:
                switch(t.get_abi()){
                    case Triple::gnu:
                        return elf_os_abi::ELF_SystemV;
                    default:
                        break;
                }
            default:
                break;
        }
        return (elf_os_abi)0;
    }

    /**
     * @brief Base class for 32bit and 64bit ELF files.
     */
    template<elf_header_class elf_class>
    class _inr_packed_ elf_header_base_class : public elf_header{
    public:
        /* Types. */
        using elf_width = std::conditional_t<elf_class == elf_header_class::ELF_32bit, uint32_t, uint64_t>;
        using elf_addr = elf_width;
        using elf_off = elf_width;
        /* End of types. */

        /**
         * @brief Basic constexpr ELF constructor.
         */
        constexpr elf_header_base_class(endian elf_endian, elf_os_abi abi, elf_type type, elf_isa arch, elf_byte elf_version = 1,
                                elf_byte abi_version = 0, uint32_t elf_header_version = 1) noexcept : 
            elf_header(elf_class, elf_endian, abi, 
                    elf_version, abi_version), e_type(type), e_machine(arch), e_version(elf_header_version), e_ehsize(sizeof(*this)){};

        /**
         * @brief Type of the ELF file, whether its relocatable, shared, executable, etc..
         */
        elf_type e_type;
        /**
         * @brief The target ISA of the ELF.
         */
        elf_isa e_machine;
        /**
         * @brief ELF Version, set it to 1 for the original version of ELF.
         */
        uint32_t e_version;
        /**
         * @brief ELF Entry point, memory address, not offset in the file.
         */
        elf_addr e_entry = 0;
        /**
         * @brief ELF Program header table offset, in-file offset.
         */
        elf_off e_phoff = 0;
        /**
         * @brief ELF Section header table offset, in-file offset.
         */
        elf_off e_shoff = 0;
        /**
         * @brief ELF Flags, depends on the target ISA.
         */
        uint32_t e_flags = 0;
        /**
         * @brief ELF Header size, handled by the template.
         */
        uint16_t e_ehsize = 0;
        /**
         * @brief ELF Program header entry size.
         */
        uint16_t e_phentsize = 0;
        /**
         * @brief Number of program header entries.
         */
        uint16_t e_phnum = 0;
        /**
         * @brief ELF Section header entry size.
         */
        uint16_t e_shentsize = 0;
        /**
         * @brief Number of section header entries.
         */
        uint16_t e_shnum = 0;
        /**
         * @brief Index of the section header table entry that contains the section names. 
         */
        uint16_t e_shstrndx = 0;

        void flip_fields() noexcept{
            e_type = (elf_type)bswap((uint16_t)e_type);
            e_machine = (elf_isa)bswap((uint16_t)e_machine);

            e_version = bswap(e_version);
            e_entry = bswap(e_entry);
            e_phoff = bswap(e_phoff);
            e_shoff = bswap(e_shoff);
            e_flags = bswap(e_flags);
            e_ehsize = bswap(e_ehsize);
            e_phentsize = bswap(e_phentsize);
            e_phnum = bswap(e_phnum);
            e_shentsize = bswap(e_shentsize);
            e_shnum = bswap(e_shnum);
            e_shstrndx = bswap(e_shstrndx);
        }
    };

    /**
     * @brief 64bit ELF header.
     */
    using elf_header_64 = elf_header_base_class<elf_header_class::ELF_64bit>;
    static_assert(sizeof(elf_header_64) == 0x40, "ELF Header 64bit isn't 64 bytes.");

    /**
     * @brief 32bit ELF header.
     */
    using elf_header_32 = elf_header_base_class<elf_header_class::ELF_32bit>;
    static_assert(sizeof(elf_header_32) == 0x34, "ELF Header 32bit isn't 52 bytes.");

    /* PROGRAM HEADER. */

    /**
     * @brief Program header type.
     */
    enum class elf_phdr_type : uint32_t{
        PT_NULL = 0x0, /**< Program header table entry unused. */
        PT_LOAD = 0x1, /**< Loadable segment. */
        PT_DYNAMIC = 0x2, /**< Dynamic linking information. */
        PT_INTERP = 0x3, /**< Interpreter information. */
        PT_NOTE = 0x4, /**< Auxiliary information. */
        PT_SHLIB = 0x5, /**< Reserved. */
        PT_PHDR = 0x6, /**< Segment containing program header table itself. */
        PT_TLS = 0x7, /**< Thread-Local Storage template. */
        PT_LOOS = 0x60000000, /**< Operating system specific. */
        PT_HIOS = 0x6FFFFFFF, /**< Operating system specific. */
        PT_LOPROC = 0x70000000, /**< Processor specific. */
        PT_HIPROC = 0x7FFFFFFF /**< Processor specific. */
    };

    /**
     * @brief Program header flags.
     */
    enum class elf_phdr_flags : uint32_t{
        PF_X = 0x1,
        PF_W = 0x2,
        PF_R = 0x4
    };

    /**
     * @brief A simple operator to make flags combining easier.
     */
    constexpr inline elf_phdr_flags operator|(elf_phdr_flags a, elf_phdr_flags b) noexcept{
        return (elf_phdr_flags)((uint32_t)a | (uint32_t)b);
    }

    /**
     * @brief Base class for the program header.
     */
    template<elf_header_class elf_class>
    class _inr_packed_ elf_program_header_base_class : public binary_header{};

    /**
     * @brief 32bit Program header class.
     */
    template<>
    class _inr_packed_ elf_program_header_base_class<elf_header_class::ELF_32bit> : public binary_header{
    public:
        using prog_width = uint32_t;
        /**
         * @brief Basic constexpr constructor, stuff not mentioned here are handled in the writer.
         */
        constexpr elf_program_header_base_class(elf_phdr_type type, prog_width vaddr, prog_width paddr,
                                                elf_phdr_flags flags, prog_width align) noexcept :
            p_type(type), p_vaddr(vaddr), p_paddr(paddr), p_flags(flags), p_align(align){};
        /**
         * @brief Type of the segment.
         */
        elf_phdr_type p_type;
        /**
         * @brief In-file offset to the segment.
         */
        prog_width p_offset = 0;
        /**
         * @brief Virtual address to load the segment in.
         */
        prog_width p_vaddr;
        /**
         * @brief Physical address to load the segment in. Usually ignored.
         */
        prog_width p_paddr;
        /**
         * @brief The size of the segment in-file.
         */
        prog_width p_filesz = 0;
        /**
         * @brief The size of the segment in-memory.
         */
        prog_width p_memsz = 0;
        /**
         * @brief Flags of the segment, R,W,X.
         */
        elf_phdr_flags p_flags;
        /**
         * @brief Power of 2 alignment of the segment. 0 or 1 mean no alignment.
         */
        prog_width p_align;
    };

    /**
     * @brief 64bit Program header class.
     */
    template<>
    class _inr_packed_ elf_program_header_base_class<elf_header_class::ELF_64bit> : public binary_header{
    public:
        using prog_width = uint64_t;
        /**
         * @brief Basic constexpr constructor, stuff not mentioned here are handled in the writer.
         */
        constexpr elf_program_header_base_class(elf_phdr_type type, prog_width vaddr, prog_width paddr,
                                                elf_phdr_flags flags, prog_width align) noexcept :
            p_type(type), p_flags(flags), p_vaddr(vaddr), p_paddr(paddr), p_align(align){};
            
        /**
         * @brief Type of the segment.
         */
        elf_phdr_type p_type;
        /**
         * @brief Flags of the segment, R,W,X.
         */
        elf_phdr_flags p_flags;
        /**
         * @brief In-file offset to the segment.
         */
        prog_width p_offset = 0;
        /**
         * @brief Virtual address to load the segment in.
         */
        prog_width p_vaddr;
        /**
         * @brief Physical address to load the segment in. Usually ignored.
         */
        prog_width p_paddr;
        /**
         * @brief The size of the segment in-file.
         */
        prog_width p_filesz = 0;
        /**
         * @brief The size of the segment in-memory.
         */
        prog_width p_memsz = 0;
        /**
         * @brief Power of 2 alignment of the segment. 0 or 1 mean no alignment.
         */
        prog_width p_align;
    };

    /**
     * @brief ELF 32bit Program header table entry.
     */
    using elf_phdr32 = elf_program_header_base_class<elf_header_class::ELF_32bit>;
    static_assert(sizeof(elf_phdr32) == 0x20, "Program header entry 32bit isn't 32 bytes.");

    /**
     * @brief ELF 64bit Program header table entry.
     */
    using elf_phdr64 = elf_program_header_base_class<elf_header_class::ELF_64bit>;
    static_assert(sizeof(elf_phdr64) == 0x38, "Program header entry 64bit isn't 56 bytes.");

    /* END OF PROGRAM HEADER. */

    /* SECTION HEADER. */

    /**
     * @brief Type of the section header.
     */
    enum class elf_shdr_type : uint32_t{
        SHT_NULL = 0x0, /**< Section header entry unused. */
        SHT_PROGBITS = 0x1, /**< Program data. */
        SHT_SYMTAB = 0x2, /**< Symbol table. */
        SHT_STRTAB = 0x3, /**< String table. */
        SHT_RELA = 0x4, /**< Relocation entries with addends. */
        SHT_HASH = 0x5, /**< Symbol hash table. */
        SHT_DYNAMIC = 0x6, /**< Dynamic linking information. */
        SHT_NOTE = 0x7, /**< Notes. */
        SHT_NOBITS = 0x8, /**< Program space with no data (like .bss). */
        SHT_REL = 0x9, /**< Relocation entries, no addends. */
        SHT_SHLIB = 0x0A, /**< Reserved. */
        SHT_DYNSYM = 0x0B, /**< Dynamic linker symbol table. */
        SHT_INIT_ARRAY = 0x0E, /**< Array of constructors. */
        SHT_FNIT_ARRAY = 0x0F, /**< Array of destructors. */
        SHT_PREINIT_ARRAY = 0x10, /**< Array of pre-constructors. */
        SHT_GROUP = 0x11, /**< Section group. */
        SHT_SYMTAB_SHNDX = 0x12, /**< Extended section indeces. */
        SHT_NUM = 0x13, /**< Number of defined types. */
        SHT_LOOS = 0x60000000 /**< OS Specific. */
    };

    /**
     * @brief Flags of the section header.
     */
    enum class elf_shdr_flags : uint32_t{
        SHF_WRITE = 0x1, /**< Writeable. */
        SHF_ALLOC = 0x2, /**< Occupies memory during execution. */
        SHF_EXECINSTR = 0x4, /**< Executable. */
        SHF_MERGE = 0x10, /**< Might be merged. */
        SHF_STRINGS = 0x20, /**< Contains null-term strings. */
        SHF_INFO_LINK = 0x40, /**< sh_info has the SHT index. */
        SHF_LINK_ORDER = 0x80, /**< Preserve order after combining. */
        SHF_OS_NONCONFORMING = 0x100, /**< Non standard OS specific handling needed. */
        SHF_GROUP = 0x200, /**< Section is a member of a group. */
        SHF_TLS = 0x400, /**< Holds thread-local data. */
        SHF_MASKOS = 0x0FF00000, /**< OS specific. */
        SHF_MASKPROC = 0xF0000000, /**< Processor specific. */
        SHF_ORDERED = 0x4000000, /**< Special ordering (Solaris). */
        SHF_EXCLUDE = 0x8000000 /**< Not included unless referenced or allocated. (Also Solaris). */
    };

    /**
     * @brief An operator to make section header flag combining easier.
     */
    constexpr inline elf_shdr_flags operator|(elf_shdr_flags a, elf_shdr_flags b) noexcept{
        return (elf_shdr_flags)((uint32_t)a | (uint32_t)b);
    }

    /**
     * @brief Base class for the section header.
     */
    template<elf_header_class elf_class>
    class _inr_packed_ elf_section_header_base_class : public binary_header{
    public:
        using shdr_width = std::conditional_t<elf_class == elf_header_class::ELF_32bit, uint32_t, uint64_t>;

        constexpr elf_section_header_base_class(elf_shdr_type type, elf_shdr_flags flags,
                                                shdr_width addr, uint32_t link, uint32_t info,
                                                shdr_width align, shdr_width entsize) noexcept :
            sh_type(type), sh_flags((shdr_width)flags), sh_addr(addr), sh_link(link), sh_info(info),
            sh_addralign(align), sh_entsize(entsize){};

        /**
         * @brief Offset in the .shstrtab section that represents the name of this section.
         */
        uint32_t sh_name = 0;
        /**
         * @brief Type of the section header.
         */
        elf_shdr_type sh_type;
        /**
         * @brief Flags of the section header.
         */
        shdr_width sh_flags;
        /**
         * @brief Virtual address of the section in memory.
         */
        shdr_width sh_addr;
        /**
         * @brief In-file offset of the segment.
         */
        shdr_width sh_offset = 0;
        /**
         * @brief Size in bytes of the segment.
         */
        shdr_width sh_size = 0;
        /**
         * @brief Depends on the type of the section.
         */
        uint32_t sh_link;
        /**
         * @brief Depends on the type of the section.
         */
        uint32_t sh_info;
        /**
         * @brief Power of 2 alignment for the segment.
         */
        shdr_width sh_addralign;
        /**
         * @brief Fixed size of each entry, 0 if not fixed size.
         */
        shdr_width sh_entsize;
    };

    /**
     * @brief ELF 32bit Section header table entry.
     */
    using elf_shdr32 = elf_section_header_base_class<elf_header_class::ELF_32bit>;
    static_assert(sizeof(elf_shdr32) == 0x28, "Section header entry 32bit isn't 40 bytes.");

    /**
     * @brief ELF 64bit Section header table entry.
     */
    using elf_shdr64 = elf_section_header_base_class<elf_header_class::ELF_64bit>;
    static_assert(sizeof(elf_shdr64) == 0x40, "Section header entry 64bit isn't 64 bytes.");

    /* END OF SECTION HEADER. */

    /* END OF ELF. */
}

#endif // INERTIA_BINARY_HPP
