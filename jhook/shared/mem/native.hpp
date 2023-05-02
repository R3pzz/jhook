#pragma once

namespace shared::x64 {
  struct dataDir {
    ulong_t _va{};
    ulong_t _size{};
  };

  static_assert( sizeof( dataDir ) == sizeof( IMAGE_DATA_DIRECTORY ),
    "sizes of sdk::detail::dataDir and IMAGE_DATA_DIRECTORY mismatch" );

  struct coffHeader {
    std::uint16_t _machine{};
    std::uint16_t _sections_cnt{};
    ulong_t _dt_stamp{};
    ulong_t _sym_table_ptr;
    ulong_t _sym_cnt{};
    std::uint16_t _opt_hdr_size{};
    std::uint16_t _characteristics{};
  };

  static_assert( sizeof( coffHeader ) == sizeof( IMAGE_FILE_HEADER ),
    "sizes of sdk::detail::coffHeader and IMAGE_FILE_HEADER mismatch" );

  struct optionalHeader {
    std::uint16_t _magic{};

    union {
      std::uint16_t _link_ver{};

      struct {
        std::uint8_t _link_ver_maj;
        std::uint8_t _link_ver_min;
      };
    };

    ulong_t _code_size{};
    ulong_t _inited_data_size{};
    ulong_t _uninited_data_size{};
    ulong_t _entry_point{};
    ulong_t _base_of_code{};
    std::uint64_t _base_of_img{};
    ulong_t _sec_align{};
    ulong_t _file_aling{};

    union {
      std::uint32_t _os_ver{};

      struct {
        std::uint16_t _os_ver_maj;
        std::uint16_t _os_ver_min;
      };
    };

    union {
      std::uint32_t _img_ver{};

      struct {
        std::uint16_t _img_ver_maj;
        std::uint16_t _img_ver_min;
      };
    };

    union {
      std::uint32_t _subsys_ver{};

      struct {
        std::uint16_t _subsys_ver_maj;
        std::uint16_t _subsys_ver_min;
      };
    };

    ulong_t _win32_ver{};
    ulong_t _img_size{};
    ulong_t _hdrs_size{};
    ulong_t _checksum{};
    std::uint16_t _subsys{};
    std::uint16_t _dll_characs{};
    std::uint64_t _stack_res_size{};
    std::uint64_t _stack_commit_size{};
    std::uint64_t _heap_res_size{};
    std::uint64_t _heap_commit_size{};
    ulong_t _ldr_flags{};
    ulong_t _rva_count{};

    union {
      dataDir _table[ 16u ]{};

      struct {
        dataDir _exp;
        dataDir _imp;
        dataDir _res;
        dataDir _exception;
        dataDir _sec;
        dataDir _basereloc;
        dataDir _dbg;
        dataDir _arch;
        dataDir _gp;
        dataDir _tls;
        dataDir _ldcfg;
        dataDir _bound_imp;
        dataDir _iat;
        dataDir _delay_imp;
        dataDir _com_desc;
      };
    } _data_dirs{};
  };

  static_assert( sizeof( optionalHeader ) == sizeof( IMAGE_OPTIONAL_HEADER64 ),
    "sizes of sdk::detail::coffHeader and IMAGE_FILE_HEADER mismatch" );

  struct ntHeader {
    ulong_t _sig{};
    coffHeader _coff{};
    optionalHeader _opt64{};
  };

  static_assert( sizeof( ntHeader ) == sizeof( IMAGE_NT_HEADERS64 ),
    "sizes of sdk::detail::nt64_hdr_t and IMAGE_NT_HEADERS64 mismatch" );

  struct dosHeader {
    std::uint16_t _magic{};
    std::uint16_t _cblp{};
    std::uint16_t _cp{};
    std::uint16_t _crlc{};
    std::uint16_t _cparhdr{};
    std::uint16_t _minalloc{};
    std::uint16_t _maxalloc{};
    std::uint16_t _ss{};
    std::uint16_t _sp{};
    std::uint16_t _csum{};
    std::uint16_t _ip{};
    std::uint16_t _cs{};
    std::uint16_t _lfarlc{};
    std::uint16_t _ovno{};
    std::uint16_t _res[ 4u ]{};
    std::uint16_t _oemid{};
    std::uint16_t _oeminfo{};
    std::uint16_t _res2[ 10u ]{};
    long _nt_reloc{};

    inline ntHeader *nt( ) const {
      return glob64Address{ this }.offset( _nt_reloc ).as<ntHeader *>( );
    }
  };

  static_assert( sizeof( dosHeader ) == sizeof( IMAGE_DOS_HEADER ),
    "sizes of sdk::detail::dos_hdr_t and IMAGE_DOS_HEADER mismatch" );
}

namespace shared {
  struct dll64Desc {
    glob64Address _base{};

    constexpr dll64Desc( ) = default;

    inline dll64Desc( const HMODULE native )
      : _base{ native } {}

    inline x64::dosHeader *dos_hdr( ) const {
      return _base.as<x64::dosHeader *>( );
    }

    inline x64::ntHeader *nt_hdr( ) const {
      return _base.as<x64::dosHeader *>( )->nt( );
    }

    inline auto end( ) const {
      return _base + nt_hdr( )->_opt64._img_size;
    }

    constexpr bool operator!( ) const {
      return !_base;
    }

    inline auto &operator=( const HMODULE other ) {
      _base = glob64Address{ other };

      return *this;
    }

    inline glob64Address query_export( const std::string_view name ) const {
      return glob64Address{ GetProcAddress( _base.as<HMODULE>( ), name.data( ) ) };
    }

    inline static dll64Desc try_find( const std::string_view name ) {
      return dll64Desc{ GetModuleHandle( name.data( ) ) };
    }
  };
}