# Automatically generated syntax database.

lappend ::dbInfo {Tcl 9.0.1 unix}
set ::dbTclVersion 9.0
set ::knownGlobals {additionalCommand argc argv argv0 auto_index auto_path configurableOOSyntax currentDir env errorCode errorInfo nagelfarPath tcl_interactive tcl_library tcl_nonwordchars tcl_patchLevel tcl_pkgPath tcl_platform tcl_rcFileName tcl_version tcl_wordchars}
set ::knownCommands {
=
@
after
append
apply
argparse
array
auto_execok
auto_import
auto_load
auto_load_index
auto_mkindex
auto_mkindex_old
auto_mkindex_parser::childhook
auto_mkindex_parser::cleanup
auto_mkindex_parser::command
auto_mkindex_parser::commandInit
auto_mkindex_parser::fullname
auto_mkindex_parser::hook
auto_mkindex_parser::indexEntry
auto_mkindex_parser::init
auto_mkindex_parser::mkindex
auto_qualify
auto_reset
binary
break
catch
cd
chan
clock
close
concat
const
continue
coroinject
coroprobe
coroutine
dict
encoding
eof
error
eval
exec
exit
expr
fblocked
fconfigure
fcopy
file
fileevent
flush
for
foreach
foreachLine
format
fpclassify
gets
glob
global
history
if
incr
info
interp
join
lappend
lassign
ledit
lindex
linsert
list
llength
lmap
load
lpop
lrange
lremove
lrepeat
lreplace
lreverse
lsearch
lseq
lset
lsort
msgcat::DefaultUnknown
msgcat::Invoke
msgcat::ListComplement
msgcat::ListEqualString
msgcat::Load
msgcat::LoadAll
msgcat::PackageLocales
msgcat::PackageNamespaceGet
msgcat::PackagePreferences
msgcat::mc
msgcat::mcexists
msgcat::mcflmset
msgcat::mcflset
msgcat::mcforgetpackage
msgcat::mcload
msgcat::mcloadedlocales
msgcat::mclocale
msgcat::mcmax
msgcat::mcmset
msgcat::mcn
msgcat::mcpackageconfig
msgcat::mcpackagelocale
msgcat::mcpackagenamespaceget
msgcat::mcpreferences
msgcat::mcset
msgcat::mcunknown
msgcat::mcutil
msgcat::mcutil::ConvertLocale
msgcat::mcutil::getpreferences
msgcat::mcutil::getsystemlocale
my
namespace
oo::DelegateName
oo::InfoClass
oo::InfoObject
oo::MixinClassDelegates
oo::Slot
oo::UnknownDefinition
oo::UnlinkLinkedCommand
oo::UpdateClassDelegatesAfterClone
oo::abstract
oo::class
oo::configurable
oo::copy
oo::define
oo::objdefine
oo::object
oo::singleton
open
package
parray
pid
pkg::create
pkg_mkIndex
proc
puts
pwd
read
readFile
regexp
regsub
rename
return
safe::AddSubDirs
safe::AliasEncodingSystem
safe::AliasExeName
safe::AliasFileSubcommand
safe::AliasGlob
safe::AliasLoad
safe::AliasSource
safe::BadSubcommand
safe::CheckFileName
safe::CheckInterp
safe::DetokPath
safe::DirInAccessPath
safe::FileInAccessPath
safe::InterpCreate
safe::InterpInit
safe::InterpNested
safe::InterpSetConfig
safe::InterpStatics
safe::Log
safe::PathToken
safe::RejectExcessColons
safe::Setup
safe::SyncAccessPath
safe::TranslatePath
safe::VarName
safe::interpAddToAccessPath
safe::interpConfigure
safe::interpCreate
safe::interpDelete
safe::interpFindInAccessPath
safe::interpInit
safe::setLogCmd
safe::setSyncMode
scan
seek
self
set
socket
source
split
string
subst
switch
tailcall
tcl::Bgerror
tcl::CopyDirectory
tcl::HistAdd
tcl::HistChange
tcl::HistClear
tcl::HistEvent
tcl::HistIndex
tcl::HistInfo
tcl::HistKeep
tcl::HistNextID
tcl::HistRedo
tcl::Lassign
tcl::Lempty
tcl::Lget
tcl::Lvarincr
tcl::Lvarincr1
tcl::Lvarpop
tcl::Lvarpop1
tcl::Lvarset
tcl::Lvarset1
tcl::Lvarset1nc
tcl::MacOSXPkgUnknown
tcl::OptAmbigous
tcl::OptBadValue
tcl::OptCheckType
tcl::OptCreateTestProc
tcl::OptCurAddr
tcl::OptCurDesc
tcl::OptCurDescFinal
tcl::OptCurSetValue
tcl::OptCurState
tcl::OptCurrentArg
tcl::OptDefaultValue
tcl::OptDoAll
tcl::OptDoOne
tcl::OptError
tcl::OptFlagUsage
tcl::OptGetPrgCounter
tcl::OptGuessType
tcl::OptHasBeenSet
tcl::OptHelp
tcl::OptHits
tcl::OptInstr
tcl::OptIsCounter
tcl::OptIsFlag
tcl::OptIsOpt
tcl::OptIsPrg
tcl::OptKeyDelete
tcl::OptKeyError
tcl::OptKeyGetDesc
tcl::OptKeyParse
tcl::OptKeyRegister
tcl::OptLengths
tcl::OptMissingValue
tcl::OptName
tcl::OptNeedValue
tcl::OptNewInst
tcl::OptNextArg
tcl::OptNextDesc
tcl::OptNormalizeOne
tcl::OptOptUsage
tcl::OptParamType
tcl::OptParse
tcl::OptProc
tcl::OptProcArgGiven
tcl::OptSelection
tcl::OptSetPrgCounter
tcl::OptState
tcl::OptTooManyArgs
tcl::OptTree
tcl::OptTreeVars
tcl::OptType
tcl::OptTypeArgs
tcl::OptValue
tcl::OptVarName
tcl::Pkg::CompareExtension
tcl::Pkg::Create
tcl::Pkg::source
tcl::SetMax
tcl::SetMin
tcl::UpdateWordBreakREs
tcl::array::anymore
tcl::array::default
tcl::array::donesearch
tcl::array::exists
tcl::array::for
tcl::array::get
tcl::array::names
tcl::array::nextelement
tcl::array::set
tcl::array::size
tcl::array::startsearch
tcl::array::statistics
tcl::array::unset
tcl::binary::decode
tcl::binary::decode::base64
tcl::binary::decode::hex
tcl::binary::decode::uuencode
tcl::binary::encode
tcl::binary::encode::base64
tcl::binary::encode::hex
tcl::binary::encode::uuencode
tcl::binary::format
tcl::binary::scan
tcl::build-info
tcl::build-info
tcl::chan::blocked
tcl::chan::close
tcl::chan::copy
tcl::chan::create
tcl::chan::eof
tcl::chan::event
tcl::chan::flush
tcl::chan::gets
tcl::chan::isbinary
tcl::chan::names
tcl::chan::pending
tcl::chan::pipe
tcl::chan::pop
tcl::chan::postevent
tcl::chan::push
tcl::chan::puts
tcl::chan::read
tcl::chan::seek
tcl::chan::tell
tcl::chan::truncate
tcl::clock::ChangeCurrentLocale
tcl::clock::ClearCaches
tcl::clock::ConvertLocalToUTC
tcl::clock::DeterminePosixDSTTime
tcl::clock::EnterLocale
tcl::clock::GetDateFields
tcl::clock::GetJulianDayFromEraYearDay
tcl::clock::GetJulianDayFromEraYearMonthDay
tcl::clock::GetJulianDayFromEraYearMonthWeekDay
tcl::clock::GetJulianDayFromEraYearWeekDay
tcl::clock::GetSystemLocale
tcl::clock::GetSystemTimeZone
tcl::clock::GuessWindowsTimeZone
tcl::clock::InitTZData
tcl::clock::IsGregorianLeapYear
tcl::clock::LoadTimeZoneFile
tcl::clock::LoadWindowsDateTimeFormats
tcl::clock::LoadZoneinfoFile
tcl::clock::LocalizeFormat
tcl::clock::ParsePosixTimeZone
tcl::clock::ProcessPosixTimeZone
tcl::clock::ReadZoneinfoFile
tcl::clock::SetupTimeZone
tcl::clock::WeekdayOnOrBefore
tcl::clock::add
tcl::clock::catch
tcl::clock::clicks
tcl::clock::format
tcl::clock::getenv
tcl::clock::mcMerge
tcl::clock::mcget
tcl::clock::mclocale
tcl::clock::mcpackagelocale
tcl::clock::microseconds
tcl::clock::milliseconds
tcl::clock::scan
tcl::clock::seconds
tcl::dict::append
tcl::dict::create
tcl::dict::exists
tcl::dict::filter
tcl::dict::for
tcl::dict::get
tcl::dict::getdef
tcl::dict::getwithdefault
tcl::dict::incr
tcl::dict::info
tcl::dict::keys
tcl::dict::lappend
tcl::dict::map
tcl::dict::merge
tcl::dict::remove
tcl::dict::replace
tcl::dict::set
tcl::dict::size
tcl::dict::unset
tcl::dict::update
tcl::dict::values
tcl::dict::with
tcl::encoding::convertfrom
tcl::encoding::convertto
tcl::encoding::dirs
tcl::encoding::names
tcl::encoding::profiles
tcl::encoding::system
tcl::file::atime
tcl::file::attributes
tcl::file::channels
tcl::file::copy
tcl::file::delete
tcl::file::dirname
tcl::file::executable
tcl::file::exists
tcl::file::extension
tcl::file::home
tcl::file::isdirectory
tcl::file::isfile
tcl::file::join
tcl::file::link
tcl::file::lstat
tcl::file::mkdir
tcl::file::mtime
tcl::file::nativename
tcl::file::normalize
tcl::file::owned
tcl::file::pathtype
tcl::file::readable
tcl::file::readlink
tcl::file::rename
tcl::file::rootname
tcl::file::separator
tcl::file::size
tcl::file::split
tcl::file::stat
tcl::file::system
tcl::file::tail
tcl::file::tempdir
tcl::file::tempfile
tcl::file::tildeexpand
tcl::file::type
tcl::file::volumes
tcl::file::writable
tcl::info::args
tcl::info::body
tcl::info::cmdcount
tcl::info::cmdtype
tcl::info::commands
tcl::info::complete
tcl::info::constant
tcl::info::consts
tcl::info::coroutine
tcl::info::default
tcl::info::errorstack
tcl::info::exists
tcl::info::frame
tcl::info::functions
tcl::info::globals
tcl::info::hostname
tcl::info::level
tcl::info::library
tcl::info::loaded
tcl::info::locals
tcl::info::nameofexecutable
tcl::info::patchlevel
tcl::info::procs
tcl::info::script
tcl::info::sharedlibextension
tcl::info::tclversion
tcl::info::vars
tcl::mathfunc::abs
tcl::mathfunc::acos
tcl::mathfunc::asin
tcl::mathfunc::atan
tcl::mathfunc::atan2
tcl::mathfunc::bool
tcl::mathfunc::ceil
tcl::mathfunc::cos
tcl::mathfunc::cosh
tcl::mathfunc::double
tcl::mathfunc::entier
tcl::mathfunc::exp
tcl::mathfunc::floor
tcl::mathfunc::fmod
tcl::mathfunc::hypot
tcl::mathfunc::int
tcl::mathfunc::isfinite
tcl::mathfunc::isinf
tcl::mathfunc::isnan
tcl::mathfunc::isnormal
tcl::mathfunc::isqrt
tcl::mathfunc::issubnormal
tcl::mathfunc::isunordered
tcl::mathfunc::log
tcl::mathfunc::log10
tcl::mathfunc::max
tcl::mathfunc::min
tcl::mathfunc::pow
tcl::mathfunc::rand
tcl::mathfunc::round
tcl::mathfunc::sin
tcl::mathfunc::sinh
tcl::mathfunc::sqrt
tcl::mathfunc::srand
tcl::mathfunc::tan
tcl::mathfunc::tanh
tcl::mathfunc::wide
tcl::mathop::eq
tcl::mathop::ge
tcl::mathop::gt
tcl::mathop::in
tcl::mathop::le
tcl::mathop::lt
tcl::mathop::ne
tcl::mathop::ni
tcl::namespace::children
tcl::namespace::code
tcl::namespace::current
tcl::namespace::delete
tcl::namespace::ensemble
tcl::namespace::eval
tcl::namespace::exists
tcl::namespace::export
tcl::namespace::forget
tcl::namespace::import
tcl::namespace::inscope
tcl::namespace::origin
tcl::namespace::parent
tcl::namespace::path
tcl::namespace::qualifiers
tcl::namespace::tail
tcl::namespace::unknown
tcl::namespace::upvar
tcl::namespace::which
tcl::pkgconfig
tcl::pkgconfig
tcl::prefix
tcl::prefix
tcl::prefix::all
tcl::prefix::longest
tcl::prefix::match
tcl::process
tcl::process
tcl::process::autopurge
tcl::process::list
tcl::process::purge
tcl::process::status
tcl::string::cat
tcl::string::compare
tcl::string::equal
tcl::string::first
tcl::string::index
tcl::string::insert
tcl::string::is
tcl::string::last
tcl::string::length
tcl::string::map
tcl::string::match
tcl::string::range
tcl::string::repeat
tcl::string::replace
tcl::string::reverse
tcl::string::tolower
tcl::string::totitle
tcl::string::toupper
tcl::string::trim
tcl::string::trimleft
tcl::string::trimright
tcl::string::wordend
tcl::string::wordstart
tcl::tm::Defaults
tcl::tm::UnknownHandler
tcl::tm::add
tcl::tm::list
tcl::tm::path
tcl::tm::remove
tcl::tm::roots
tcl::unsupported::assemble
tcl::unsupported::clock::configure
tcl::unsupported::corotype
tcl::unsupported::disassemble
tcl::unsupported::getbytecode
tcl::unsupported::icu
tcl::unsupported::icu::Init
tcl::unsupported::icu::LogError
tcl::unsupported::icu::MappedIcuNames
tcl::unsupported::icu::UnmappedIcuNames
tcl::unsupported::icu::UnmappedTclNames
tcl::unsupported::icu::aliases
tcl::unsupported::icu::converters
tcl::unsupported::icu::convertfrom
tcl::unsupported::icu::convertto
tcl::unsupported::icu::detect
tcl::unsupported::icu::icuToTcl
tcl::unsupported::icu::normalize
tcl::unsupported::icu::tclToIcu
tcl::unsupported::loadIcu
tcl::unsupported::representation
tcl::zipfs::Find
tcl::zipfs::canonical
tcl::zipfs::exists
tcl::zipfs::find
tcl::zipfs::info
tcl::zipfs::list
tcl::zipfs::lmkimg
tcl::zipfs::lmkzip
tcl::zipfs::mkimg
tcl::zipfs::mkkey
tcl::zipfs::mkzip
tcl::zipfs::mount
tcl::zipfs::mountdata
tcl::zipfs::root
tcl::zipfs::tcl_library_init
tcl::zipfs::unmount
tclLog
tclPkgSetup
tclPkgUnknown
tcl_endOfWord
tcl_findLibrary
tcl_startOfNextWord
tcl_startOfPreviousWord
tcl_wordBreakAfter
tcl_wordBreakBefore
tell
throw
time
timerate
trace
try
unknown
unload
unset
update
uplevel
upvar
variable
vwait
while
writeFile
yield
yieldto
zipfs
zlib
zlib::pkgconfig
}
set ::knownPackages {Tcl TclOO msgcat zlib}
set ::syntax(_stdclass_oo) {s x*}
set {::syntax(_stdclass_oo create)} {dc=_obj,_stdclass_oo x?}
set {::syntax(_stdclass_oo destroy)} 0
set {::syntax(_stdclass_oo new)} 0
set {::syntax(_stdclass_oo variable)} n*
set {::syntax(_stdclass_oo varname)} v
set ::syntax(after) {r 1}
set ::syntax(append) {n x*}
set ::syntax(apply) {x x*}
set ::syntax(argparse) {r 0}
set ::syntax(array) {s v x?}
set {::syntax(array default)} {s l x?}
set {::syntax(array default exists)} l=array
set {::syntax(array default get)} v=array
set {::syntax(array default set)} {v=array x}
set {::syntax(array default unset)} v=array
set {::syntax(array exists)} l=array
set {::syntax(array for)} {nl v=array c}
set {::syntax(array names)} {v=array x? x?}
set {::syntax(array set)} {n=array x}
set {::syntax(array size)} v=array
set {::syntax(array statistics)} v=array
set {::syntax(array unset)} {l x?}
set ::syntax(auto_execok) 1
set ::syntax(auto_import) 1
set ::syntax(auto_load) {r 1 2}
set ::syntax(auto_load_index) 0
set ::syntax(auto_mkindex) {r 1}
set ::syntax(auto_mkindex_old) {r 1}
set ::syntax(auto_mkindex_parser::childhook) 1
set ::syntax(auto_mkindex_parser::cleanup) 0
set ::syntax(auto_mkindex_parser::command) 3
set ::syntax(auto_mkindex_parser::commandInit) 3
set ::syntax(auto_mkindex_parser::fullname) 1
set ::syntax(auto_mkindex_parser::hook) 1
set ::syntax(auto_mkindex_parser::indexEntry) 1
set ::syntax(auto_mkindex_parser::init) 0
set ::syntax(auto_mkindex_parser::mkindex) 1
set ::syntax(auto_qualify) 2
set ::syntax(auto_reset) 0
set ::syntax(binary) {s x*}
set {::syntax(binary decode)} {s x*}
set {::syntax(binary decode base64)} {o* x}
set {::syntax(binary decode hex)} {o* x}
set {::syntax(binary decode uuencode)} {o* x}
set {::syntax(binary encode)} {s x*}
set {::syntax(binary encode base64)} {p* x}
set {::syntax(binary encode hex)} x
set {::syntax(binary encode uuencode)} {p* x}
set {::syntax(binary scan)} {x x n n*}
set ::syntax(break) 0
set ::syntax(catch) {c n? n?}
set ::syntax(cd) {r 0 1}
set ::syntax(chan) {s x*}
set {::syntax(chan blocked)} x
set {::syntax(chan close)} {x x?}
set {::syntax(chan configure)} {x o. x. p*}
set {::syntax(chan copy)} {x x p*}
set {::syntax(chan create)} {x x}
set {::syntax(chan eof)} x
set {::syntax(chan event)} {x x cg?}
set {::syntax(chan flush)} x
set {::syntax(chan gets)} {x n?}
set {::syntax(chan isbinary)} x
set {::syntax(chan names)} x?
set {::syntax(chan pending)} {x x}
set {::syntax(chan pipe)} 0
set {::syntax(chan pop)} x
set {::syntax(chan postevent)} {x x}
set {::syntax(chan push)} {x c}
set {::syntax(chan puts)} {1: x : o? x x?}
set {::syntax(chan read)} {x x?}
set {::syntax(chan seek)} {r 2 3}
set {::syntax(chan tell)} 1
set {::syntax(chan truncate)} {x x?}
set ::syntax(clock) {s x*}
set {::syntax(clock clicks)} o?
set {::syntax(clock format)} {x p*}
set {::syntax(clock scan)} {x p*}
set {::syntax(clock seconds)} 0
set ::syntax(close) {x x?}
set ::syntax(concat) {r 0}
set ::syntax(const) {n x}
set ::syntax(continue) 0
set ::syntax(coroinject) {x x x*}
set ::syntax(coroprobe) {x x x*}
set ::syntax(coroutine) {x x x*}
set ::syntax(dict) {s x x*}
set {::syntax(dict append)} {n x x*}
set {::syntax(dict create)} x&x*
set {::syntax(dict exists)} {x x x*}
set {::syntax(dict filter)} {x x x*}
set {::syntax(dict filter key)} x*
set {::syntax(dict filter script)} {nl c}
set {::syntax(dict filter value)} x*
set {::syntax(dict for)} {nl x c}
set {::syntax(dict incr)} {n x x*}
set {::syntax(dict info)} x
set {::syntax(dict keys)} {x x?}
set {::syntax(dict lappend)} {n x x*}
set {::syntax(dict map)} {nl x c}
set {::syntax(dict remove)} {x x*}
set {::syntax(dict replace)} {x x*}
set {::syntax(dict set)} {n x x x*}
set {::syntax(dict size)} x
set {::syntax(dict unset)} {n x x*}
set {::syntax(dict update)} {l x n x&n* c}
set {::syntax(dict values)} {x x?}
set {::syntax(dict with)} {l x* c}
set ::syntax(encoding) {s x*}
set {::syntax(encoding convertfrom)} {p* x? x}
set {::syntax(encoding convertto)} {p* x? x}
set {::syntax(encoding names)} 0
set {::syntax(encoding system)} {r 0 1}
set ::syntax(eof) 1
set ::syntax(error) {r 1 3}
set ::syntax(exec) {o* x x*}
set ::syntax(exit) {r 0 1}
set ::syntax(fblocked) 1
set ::syntax(fconfigure) {x o. x. p*}
set ::syntax(fcopy) {x x p*}
set ::syntax(file) {s x*}
set {::syntax(file atime)} {x x?}
set {::syntax(file attributes)} {x o. x. p*}
set {::syntax(file channels)} x?
set {::syntax(file copy)} {o* x x x*}
set {::syntax(file delete)} {o* x*}
set {::syntax(file dirname)} x
set {::syntax(file executable)} x
set {::syntax(file exists)} x
set {::syntax(file extension)} x
set {::syntax(file home)} x?
set {::syntax(file isdirectory)} x
set {::syntax(file isfile)} x
set {::syntax(file join)} {x x*}
set {::syntax(file link)} {o? x x?}
set {::syntax(file lstat)} {x n?}
set {::syntax(file mkdir)} x*
set {::syntax(file mtime)} {x x?}
set {::syntax(file nativename)} x
set {::syntax(file normalize)} x
set {::syntax(file owned)} x
set {::syntax(file pathtype)} x
set {::syntax(file readable)} x
set {::syntax(file readlink)} x
set {::syntax(file rename)} {o* x x x*}
set {::syntax(file rootname)} x
set {::syntax(file separator)} x?
set {::syntax(file size)} x
set {::syntax(file split)} x
set {::syntax(file stat)} {x n?}
set {::syntax(file system)} x
set {::syntax(file tail)} x
set {::syntax(file tempdir)} x?
set {::syntax(file tempfile)} {n? x?}
set {::syntax(file tildeexpand)} x
set {::syntax(file type)} x
set {::syntax(file volumes)} 0
set {::syntax(file writable)} x
set ::syntax(fileevent) {x x x?}
set ::syntax(flush) 1
set ::syntax(for) {c E c c}
set ::syntax(foreachLine) {n x c}
set ::syntax(format) {r 1}
set ::syntax(fpclassify) x
set ::syntax(gets) {x n?}
set ::syntax(glob) {o* x*}
set ::syntax(history) {s x*}
set ::syntax(if) {e c}
set ::syntax(incr) {n x?}
set ::syntax(info) {s x*}
set {::syntax(info class)} {s x x*}
set {::syntax(info cmdtype)} x
set {::syntax(info constant)} l
set {::syntax(info consts)} x?
set {::syntax(info coroutine)} 0
set {::syntax(info default)} {x x n}
set {::syntax(info exists)} l
set {::syntax(info object)} {s x x*}
set ::syntax(interp) {s x*}
set {::syntax(interp cancel)} {o* x? x?}
set {::syntax(interp invokehidden)} {x o* x x*}
set ::syntax(join) {r 1 2}
set ::syntax(lappend) {n x*}
set ::syntax(lassign) {x n*}
set ::syntax(ledit) {v x x x*}
set ::syntax(lindex) {r 1}
set ::syntax(linsert) {r 2}
set ::syntax(list) {r 0}
set ::syntax(llength) 1
set ::syntax(lmap) {n x c}
set ::syntax(load) {r 1 3}
set ::syntax(lpop) {v x*}
set ::syntax(lrange) 3
set ::syntax(lremove) {x x*}
set ::syntax(lrepeat) {r 1}
set ::syntax(lreplace) {r 3}
set ::syntax(lreverse) 1
set ::syntax(lsearch) {o* x x}
set ::syntax(lseq) {x x*}
set ::syntax(lset) {n x x x*}
set ::syntax(lsort) {o* x}
set ::syntax(msgcat::DefaultUnknown) {r 2}
set ::syntax(msgcat::Invoke) {r 2 5}
set ::syntax(msgcat::ListComplement) {r 2 3}
set ::syntax(msgcat::ListEqualString) 2
set ::syntax(msgcat::Load) {r 2 3}
set ::syntax(msgcat::LoadAll) 1
set ::syntax(msgcat::PackageLocales) 1
set ::syntax(msgcat::PackageNamespaceGet) 0
set ::syntax(msgcat::PackagePreferences) 1
set ::syntax(msgcat::mc) {r 0}
set ::syntax(msgcat::mcexists) {r 0}
set ::syntax(msgcat::mcflmset) 1
set ::syntax(msgcat::mcflset) {r 1 2}
set ::syntax(msgcat::mcforgetpackage) 0
set ::syntax(msgcat::mcload) 1
set ::syntax(msgcat::mcloadedlocales) 1
set ::syntax(msgcat::mclocale) {r 0}
set ::syntax(msgcat::mcmax) {r 0}
set ::syntax(msgcat::mcmset) 2
set ::syntax(msgcat::mcn) {r 2}
set ::syntax(msgcat::mcpackageconfig) {r 2 3}
set ::syntax(msgcat::mcpackagelocale) {r 1}
set ::syntax(msgcat::mcpackagenamespaceget) 0
set ::syntax(msgcat::mcpreferences) {r 0}
set ::syntax(msgcat::mcset) {r 2 3}
set ::syntax(msgcat::mcunknown) {r 0}
set ::syntax(msgcat::mcutil::ConvertLocale) 1
set ::syntax(msgcat::mcutil::getpreferences) 1
set ::syntax(msgcat::mcutil::getsystemlocale) 0
set ::syntax(my) {s x*}
set {::syntax(my variable)} n*
set ::syntax(namespace) {s x*}
set {::syntax(namespace code)} c
set {::syntax(namespace import)} {o* x*}
set {::syntax(namespace which)} {o* x?}
set ::syntax(oo::DelegateName) 1
set ::syntax(oo::MixinClassDelegates) 1
set ::syntax(oo::UnlinkLinkedCommand) {r 1}
set ::syntax(oo::UpdateClassDelegatesAfterClone) 2
set ::syntax(oo::class) {s x*}
set {::syntax(oo::class create)} {do=_stdclass_oo cn?}
set {::syntax(oo::class create::class)} x
set {::syntax(oo::class create::classmethod)} dm
set {::syntax(oo::class create::constructor)} dk
set {::syntax(oo::class create::deletemethod)} {x x*}
set {::syntax(oo::class create::destructor)} dd
set {::syntax(oo::class create::export)} {x x*}
set {::syntax(oo::class create::filter)} {o? x*}
set {::syntax(oo::class create::forward)} {x x x*}
set {::syntax(oo::class create::initialise)} x
set {::syntax(oo::class create::method)} dm
set {::syntax(oo::class create::mixin)} {o? di}
set {::syntax(oo::class create::renamemethod)} {x x}
set {::syntax(oo::class create::self)} x*
set {::syntax(oo::class create::superclass)} {o? di}
set {::syntax(oo::class create::unexport)} {x x*}
set {::syntax(oo::class create::variable)} {o? div*}
set ::syntax(oo::copy) {x x?}
set ::syntax(oo::define) {2: do cn : do s x x*}
set {::syntax(oo::define class)} x
set {::syntax(oo::define constructor)} dk
set {::syntax(oo::define deletemethod)} {x x*}
set {::syntax(oo::define destructor)} dd
set {::syntax(oo::define export)} {x x*}
set {::syntax(oo::define filter)} {o? x*}
set {::syntax(oo::define forward)} {x x x*}
set {::syntax(oo::define method)} dm
set {::syntax(oo::define mixin)} {o? di}
set {::syntax(oo::define renamemethod)} {x x}
set {::syntax(oo::define self)} x*
set {::syntax(oo::define superclass)} {o? di}
set {::syntax(oo::define unexport)} {x x*}
set {::syntax(oo::define variable)} {o? div*}
set ::syntax(oo::define::class) x
set ::syntax(oo::define::constructor) dk
set ::syntax(oo::define::deletemethod) {x x*}
set ::syntax(oo::define::destructor) dd
set ::syntax(oo::define::export) {x x*}
set ::syntax(oo::define::filter) {o? x*}
set ::syntax(oo::define::forward) {x x x*}
set ::syntax(oo::define::method) dm
set ::syntax(oo::define::mixin) {o? di}
set ::syntax(oo::define::renamemethod) {x x}
set ::syntax(oo::define::self) x*
set ::syntax(oo::define::superclass) {o? di}
set ::syntax(oo::define::unexport) {x x*}
set ::syntax(oo::define::variable) {o? div*}
set ::syntax(oo::objdefine) {2: do cn : do s x x*}
set {::syntax(oo::objdefine class)} x
set {::syntax(oo::objdefine constructor)} dk
set {::syntax(oo::objdefine deletemethod)} {x x*}
set {::syntax(oo::objdefine destructor)} dd
set {::syntax(oo::objdefine export)} {x x*}
set {::syntax(oo::objdefine filter)} {o? x*}
set {::syntax(oo::objdefine forward)} {x x x*}
set {::syntax(oo::objdefine method)} dm
set {::syntax(oo::objdefine mixin)} {o? di}
set {::syntax(oo::objdefine renamemethod)} {x x}
set {::syntax(oo::objdefine self)} x*
set {::syntax(oo::objdefine superclass)} {o? di}
set {::syntax(oo::objdefine unexport)} {x x*}
set {::syntax(oo::objdefine variable)} {o? div*}
set ::syntax(oo::objdefine::class) x
set ::syntax(oo::objdefine::constructor) dk
set ::syntax(oo::objdefine::deletemethod) {x x*}
set ::syntax(oo::objdefine::destructor) dd
set ::syntax(oo::objdefine::export) {x x*}
set ::syntax(oo::objdefine::filter) {o? x*}
set ::syntax(oo::objdefine::forward) {x x x*}
set ::syntax(oo::objdefine::method) dm
set ::syntax(oo::objdefine::mixin) {o? di}
set ::syntax(oo::objdefine::renamemethod) {x x}
set ::syntax(oo::objdefine::self) x*
set ::syntax(oo::objdefine::superclass) {o? di}
set ::syntax(oo::objdefine::unexport) {x x*}
set ::syntax(oo::objdefine::variable) {o? div*}
set ::syntax(oo::object) {s x*}
set ::syntax(open) {r 1 3}
set ::syntax(package) {s x*}
set {::syntax(package require)} {o* x x*}
set ::syntax(parray) {v x?}
set ::syntax(pid) {r 0 1}
set ::syntax(pkg_mkIndex) {r 0}
set ::syntax(proc) dp
set ::syntax(puts) {1: x : o? x x?}
set ::syntax(pwd) 0
set ::syntax(read) {r 1 2}
set ::syntax(readFile) {x x?}
set ::syntax(regexp) {o* re x n*}
set ::syntax(regsub) {o* re x x n?}
set ::syntax(rename) 2
set ::syntax(return) {p* x?}
set ::syntax(safe::AddSubDirs) 1
set ::syntax(safe::AliasEncodingSystem) {r 1}
set ::syntax(safe::AliasExeName) 1
set ::syntax(safe::AliasFileSubcommand) 3
set ::syntax(safe::AliasGlob) {r 1}
set ::syntax(safe::AliasLoad) {r 2}
set ::syntax(safe::AliasSource) {r 1}
set ::syntax(safe::BadSubcommand) {r 3}
set ::syntax(safe::CheckFileName) 2
set ::syntax(safe::CheckInterp) 1
set ::syntax(safe::DetokPath) 2
set ::syntax(safe::DirInAccessPath) 2
set ::syntax(safe::FileInAccessPath) 2
set ::syntax(safe::InterpCreate) 7
set ::syntax(safe::InterpInit) 7
set ::syntax(safe::InterpNested) 0
set ::syntax(safe::InterpSetConfig) 7
set ::syntax(safe::InterpStatics) 0
set ::syntax(safe::Log) {r 0}
set ::syntax(safe::PathToken) 1
set ::syntax(safe::RejectExcessColons) 1
set ::syntax(safe::Setup) 0
set ::syntax(safe::SyncAccessPath) 1
set ::syntax(safe::TranslatePath) 2
set ::syntax(safe::VarName) 1
set ::syntax(safe::interpAddToAccessPath) 2
set ::syntax(safe::interpConfigure) {r 0}
set ::syntax(safe::interpCreate) {r 0}
set ::syntax(safe::interpDelete) 1
set ::syntax(safe::interpFindInAccessPath) 2
set ::syntax(safe::interpInit) {r 0}
set ::syntax(safe::setLogCmd) {r 0}
set ::syntax(safe::setSyncMode) {r 0}
set ::syntax(scan) {x x n*}
set ::syntax(seek) {r 2 3}
set ::syntax(self) s
set ::syntax(set) {1: v=scalar : n=scalar x}
set ::syntax(socket) {r 2}
set ::syntax(source) {p* x}
set ::syntax(split) {r 1 2}
set ::syntax(string) {s x x*}
set {::syntax(string compare)} {o* x x}
set {::syntax(string equal)} {o* x x}
set {::syntax(string first)} {r 2 3}
set {::syntax(string index)} 2
set {::syntax(string insert)} 3
set {::syntax(string is)} {s o* x}
set {::syntax(string last)} {r 2 3}
set {::syntax(string length)} 1
set {::syntax(string map)} {o? x x}
set {::syntax(string match)} {o? x x}
set {::syntax(string range)} 3
set {::syntax(string repeat)} 2
set {::syntax(string replace)} {r 3 4}
set {::syntax(string reverse)} 1
set {::syntax(string tolower)} {r 1 3}
set {::syntax(string totitle)} {r 1 3}
set {::syntax(string toupper)} {r 1 3}
set {::syntax(string trim)} {r 1 2}
set {::syntax(string trimleft)} {r 1 2}
set {::syntax(string trimright)} {r 1 2}
set {::syntax(string wordend)} 2
set {::syntax(string wordstart)} 2
set ::syntax(subst) {o* x}
set ::syntax(switch) x*
set ::syntax(tcl::CopyDirectory) 3
set ::syntax(tcl::HistAdd) {r 1 2}
set ::syntax(tcl::HistChange) {r 1 2}
set ::syntax(tcl::HistClear) 0
set ::syntax(tcl::HistEvent) {r 0 1}
set ::syntax(tcl::HistIndex) 1
set ::syntax(tcl::HistInfo) {r 0 1}
set ::syntax(tcl::HistKeep) {r 0 1}
set ::syntax(tcl::HistNextID) 0
set ::syntax(tcl::HistRedo) {r 0 1}
set ::syntax(tcl::Lassign) {r 1}
set ::syntax(tcl::Lempty) 1
set ::syntax(tcl::Lget) 2
set ::syntax(tcl::Lvarincr) {r 2 3}
set ::syntax(tcl::Lvarincr1) {r 2 3}
set ::syntax(tcl::Lvarpop) 1
set ::syntax(tcl::Lvarpop1) 1
set ::syntax(tcl::Lvarset) 3
set ::syntax(tcl::Lvarset1) 3
set ::syntax(tcl::Lvarset1nc) 3
set ::syntax(tcl::MacOSXPkgUnknown) {r 2}
set ::syntax(tcl::OptAmbigous) 2
set ::syntax(tcl::OptBadValue) {r 2 3}
set ::syntax(tcl::OptCheckType) {r 2 3}
set ::syntax(tcl::OptCreateTestProc) 0
set ::syntax(tcl::OptCurAddr) {r 1 2}
set ::syntax(tcl::OptCurDesc) 1
set ::syntax(tcl::OptCurDescFinal) 1
set ::syntax(tcl::OptCurSetValue) 2
set ::syntax(tcl::OptCurState) 1
set ::syntax(tcl::OptCurrentArg) 1
set ::syntax(tcl::OptDefaultValue) 1
set ::syntax(tcl::OptDoAll) 2
set ::syntax(tcl::OptDoOne) 3
set ::syntax(tcl::OptError) {r 2 3}
set ::syntax(tcl::OptFlagUsage) 2
set ::syntax(tcl::OptGetPrgCounter) 1
set ::syntax(tcl::OptGuessType) 1
set ::syntax(tcl::OptHasBeenSet) 1
set ::syntax(tcl::OptHelp) 1
set ::syntax(tcl::OptHits) 2
set ::syntax(tcl::OptInstr) 1
set ::syntax(tcl::OptIsCounter) 1
set ::syntax(tcl::OptIsFlag) 1
set ::syntax(tcl::OptIsOpt) 1
set ::syntax(tcl::OptIsPrg) 1
set ::syntax(tcl::OptKeyDelete) 1
set ::syntax(tcl::OptKeyError) {r 2 3}
set ::syntax(tcl::OptKeyGetDesc) 1
set ::syntax(tcl::OptKeyParse) 2
set ::syntax(tcl::OptKeyRegister) {r 1 2}
set ::syntax(tcl::OptLengths) 4
set ::syntax(tcl::OptMissingValue) 1
set ::syntax(tcl::OptName) 1
set ::syntax(tcl::OptNeedValue) 1
set ::syntax(tcl::OptNewInst) 5
set ::syntax(tcl::OptNextArg) 1
set ::syntax(tcl::OptNextDesc) 1
set ::syntax(tcl::OptNormalizeOne) 1
set ::syntax(tcl::OptOptUsage) {r 1 2}
set ::syntax(tcl::OptParamType) 1
set ::syntax(tcl::OptParse) 2
set ::syntax(tcl::OptProc) 3
set ::syntax(tcl::OptProcArgGiven) 1
set ::syntax(tcl::OptSelection) 1
set ::syntax(tcl::OptSetPrgCounter) 2
set ::syntax(tcl::OptState) 1
set ::syntax(tcl::OptTooManyArgs) 2
set ::syntax(tcl::OptTree) 4
set ::syntax(tcl::OptTreeVars) {r 2 3}
set ::syntax(tcl::OptType) 1
set ::syntax(tcl::OptTypeArgs) 1
set ::syntax(tcl::OptValue) 1
set ::syntax(tcl::OptVarName) 1
set ::syntax(tcl::Pkg::CompareExtension) {r 1 2}
set ::syntax(tcl::Pkg::Create) {r 0}
set ::syntax(tcl::Pkg::source) 1
set ::syntax(tcl::SetMax) 2
set ::syntax(tcl::SetMin) 2
set ::syntax(tcl::UpdateWordBreakREs) {r 0}
set ::syntax(tcl::clock::ChangeCurrentLocale) {r 0}
set ::syntax(tcl::clock::ClearCaches) 0
set ::syntax(tcl::clock::DeterminePosixDSTTime) 3
set ::syntax(tcl::clock::EnterLocale) 1
set ::syntax(tcl::clock::GetJulianDayFromEraYearDay) 2
set ::syntax(tcl::clock::GetJulianDayFromEraYearMonthWeekDay) 2
set ::syntax(tcl::clock::GetSystemLocale) 0
set ::syntax(tcl::clock::GetSystemTimeZone) 0
set ::syntax(tcl::clock::GuessWindowsTimeZone) 0
set ::syntax(tcl::clock::InitTZData) 0
set ::syntax(tcl::clock::IsGregorianLeapYear) 1
set ::syntax(tcl::clock::LoadTimeZoneFile) 1
set ::syntax(tcl::clock::LoadWindowsDateTimeFormats) 1
set ::syntax(tcl::clock::LoadZoneinfoFile) 1
set ::syntax(tcl::clock::LocalizeFormat) 3
set ::syntax(tcl::clock::ParsePosixTimeZone) 1
set ::syntax(tcl::clock::ProcessPosixTimeZone) 1
set ::syntax(tcl::clock::ReadZoneinfoFile) 2
set ::syntax(tcl::clock::SetupTimeZone) {r 1 2}
set ::syntax(tcl::clock::WeekdayOnOrBefore) 2
set ::syntax(tcl::clock::mcMerge) 1
set ::syntax(tcl::clock::mcget) 1
set ::syntax(tcl::clock::mclocale) {r 0}
set ::syntax(tcl::clock::mcpackagelocale) {r 1}
set ::syntax(tcl::mathfunc::abs) 1
set ::syntax(tcl::mathfunc::acos) 1
set ::syntax(tcl::mathfunc::asin) 1
set ::syntax(tcl::mathfunc::atan) 1
set ::syntax(tcl::mathfunc::atan2) 2
set ::syntax(tcl::mathfunc::bool) 1
set ::syntax(tcl::mathfunc::ceil) 1
set ::syntax(tcl::mathfunc::cos) 1
set ::syntax(tcl::mathfunc::cosh) 1
set ::syntax(tcl::mathfunc::double) 1
set ::syntax(tcl::mathfunc::entier) 1
set ::syntax(tcl::mathfunc::exp) 1
set ::syntax(tcl::mathfunc::floor) 1
set ::syntax(tcl::mathfunc::fmod) 2
set ::syntax(tcl::mathfunc::hypot) 2
set ::syntax(tcl::mathfunc::int) 1
set ::syntax(tcl::mathfunc::isfinite) 1
set ::syntax(tcl::mathfunc::isinf) 1
set ::syntax(tcl::mathfunc::isnan) 1
set ::syntax(tcl::mathfunc::isnormal) 1
set ::syntax(tcl::mathfunc::isqrt) 1
set ::syntax(tcl::mathfunc::issubnormal) 1
set ::syntax(tcl::mathfunc::isunordered) 2
set ::syntax(tcl::mathfunc::log) 1
set ::syntax(tcl::mathfunc::log10) 1
set ::syntax(tcl::mathfunc::max) {r 1}
set ::syntax(tcl::mathfunc::min) {r 1}
set ::syntax(tcl::mathfunc::pow) 2
set ::syntax(tcl::mathfunc::rand) 0
set ::syntax(tcl::mathfunc::round) 1
set ::syntax(tcl::mathfunc::sin) 1
set ::syntax(tcl::mathfunc::sinh) 1
set ::syntax(tcl::mathfunc::sqrt) 1
set ::syntax(tcl::mathfunc::srand) 1
set ::syntax(tcl::mathfunc::tan) 1
set ::syntax(tcl::mathfunc::tanh) 1
set ::syntax(tcl::mathfunc::wide) 1
set ::syntax(tcl::mathop::!) 1
set ::syntax(tcl::mathop::!=) 2
set ::syntax(tcl::mathop::%) 2
set ::syntax(tcl::mathop::&) {r 0}
set ::syntax(tcl::mathop::*) {r 0}
set ::syntax(tcl::mathop::**) {r 0}
set ::syntax(tcl::mathop::+) {r 0}
set ::syntax(tcl::mathop::-) {r 1}
set ::syntax(tcl::mathop::/) {r 1}
set ::syntax(tcl::mathop::<) {r 0}
set ::syntax(tcl::mathop::<<) 2
set ::syntax(tcl::mathop::<=) {r 0}
set ::syntax(tcl::mathop::==) {r 0}
set ::syntax(tcl::mathop::>) {r 0}
set ::syntax(tcl::mathop::>=) {r 0}
set ::syntax(tcl::mathop::>>) 2
set ::syntax(tcl::mathop::^) {r 0}
set ::syntax(tcl::mathop::eq) {r 0}
set ::syntax(tcl::mathop::ge) {r 0}
set ::syntax(tcl::mathop::gt) {r 0}
set ::syntax(tcl::mathop::in) 2
set ::syntax(tcl::mathop::le) {r 0}
set ::syntax(tcl::mathop::lt) {r 0}
set ::syntax(tcl::mathop::ne) 2
set ::syntax(tcl::mathop::ni) 2
set ::syntax(tcl::mathop::|) {r 0}
set ::syntax(tcl::mathop::~) 1
set ::syntax(tcl::prefix) {s x*}
set {::syntax(tcl::prefix all)} {x x}
set {::syntax(tcl::prefix longest)} {x x}
set {::syntax(tcl::prefix match)} {o* x x}
set ::syntax(tcl::process) {s x*}
set {::syntax(tcl::process autoperge)} x?
set {::syntax(tcl::process list)} 0
set {::syntax(tcl::process purge)} x?
set {::syntax(tcl::process status)} {o* x?}
set ::syntax(tcl::tm::Defaults) 0
set ::syntax(tcl::tm::UnknownHandler) {r 2}
set ::syntax(tcl::tm::add) {r 0}
set ::syntax(tcl::tm::list) 0
set ::syntax(tcl::tm::remove) {r 0}
set ::syntax(tcl::tm::roots) 1
set ::syntax(tcl::unsupported::icu::Init) 0
set ::syntax(tcl::unsupported::icu::LogError) 1
set ::syntax(tcl::unsupported::icu::MappedIcuNames) {r 0 1}
set ::syntax(tcl::unsupported::icu::UnmappedIcuNames) {r 0 1}
set ::syntax(tcl::unsupported::icu::UnmappedTclNames) {r 0 1}
set ::syntax(tcl::unsupported::icu::icuToTcl) 1
set ::syntax(tcl::unsupported::icu::tclToIcu) 1
set ::syntax(tcl::zipfs::Find) 1
set ::syntax(tcl::zipfs::find) 1
set ::syntax(tclLog) 1
set ::syntax(tclPkgSetup) 4
set ::syntax(tclPkgUnknown) {r 1}
set ::syntax(tcl_endOfWord) 2
set ::syntax(tcl_findLibrary) 6
set ::syntax(tcl_startOfNextWord) 2
set ::syntax(tcl_startOfPreviousWord) 2
set ::syntax(tcl_wordBreakAfter) 2
set ::syntax(tcl_wordBreakBefore) 2
set ::syntax(tell) 1
set ::syntax(throw) 2
set ::syntax(time) {c x?}
set ::syntax(timerate) {o* c x? x?}
set ::syntax(trace) {s x x*}
set {::syntax(trace add)} {s x x x}
set {::syntax(trace add command)} {x x c3}
set {::syntax(trace add execution)} {x s c2}
set {::syntax(trace add execution leave)} c4
set {::syntax(trace add execution leavestep)} c4
set {::syntax(trace add variable)} {v x c3}
set {::syntax(trace info)} {s x x x}
set {::syntax(trace info command)} x
set {::syntax(trace info execution)} x
set {::syntax(trace info variable)} v
set {::syntax(trace remove)} {s x x x}
set {::syntax(trace remove command)} {x x x}
set {::syntax(trace remove execution)} {x x x}
set {::syntax(trace remove variable)} {v x x}
set ::syntax(try) {c x*}
set ::syntax(unknown) {r 0}
set ::syntax(unload) {o* x x*}
set ::syntax(unset) {o* l l*}
set ::syntax(update) s.
set ::syntax(vwait) {1: n : o* n*}
set ::syntax(while) {E c}
set ::syntax(writeFile) {r 2 3}
set ::syntax(yield) x?
set ::syntax(yieldto) {x x*}
set ::syntax(zipfs) {s x*}
set {::syntax(zipfs canonical)} {r 1 3}
set {::syntax(zipfs exists)} 1
set {::syntax(zipfs find)} 1
set {::syntax(zipfs info)} 1
set {::syntax(zipfs list)} {o* x?}
set {::syntax(zipfs lmkimg)} {r 2 4}
set {::syntax(zipfs lmkzip)} {r 2 3}
set {::syntax(zipfs mkimg)} {r 2 5}
set {::syntax(zipfs mkkey)} 1
set {::syntax(zipfs mkzip)} {r 2 4}
set {::syntax(zipfs mount)} {r 0 3}
set {::syntax(zipfs root)} 0
set {::syntax(zipfs unmount)} 1
set ::syntax(zlib) {s x*}
set {::syntax(zlib adler32)} {x x?}
set {::syntax(zlib compress)} {x x?}
set {::syntax(zlib crc32)} {x x?}
set {::syntax(zlib decompress)} {x x?}
set {::syntax(zlib deflate)} {x x?}
set {::syntax(zlib gunzip)} {x p*}
set {::syntax(zlib gzip)} {x p*}
set {::syntax(zlib inflate)} {x x?}
set {::syntax(zlib push)} {s x p*}
set {::syntax(zlib stream)} {s x*}
set {::syntax(zlib stream compress)} p*
set {::syntax(zlib stream decompress)} p*
set {::syntax(zlib stream deflate)} p*
set {::syntax(zlib stream gunzip)} 0
set {::syntax(zlib stream gzip)} p*
set {::syntax(zlib stream inflate)} p*

set {::return(_stdclass_oo create)} _obj,_stdclass_oo
set {::return(_stdclass_oo new)} _obj,_stdclass_oo
set {::return(_stdclass_oo varname)} varName
set ::return(linsert) list
set ::return(list) list
set ::return(llength) int
set ::return(lrange) list
set ::return(lreplace) list
set ::return(lsort) list
set {::return(namespace code)} script

set ::subCmd(_stdclass_oo) {create new destroy variable varname}
set ::subCmd(array) {anymore default donesearch exists for get names nextelement set size startsearch statistics unset}
set {::subCmd(array default)} {exists get set unset}
set ::subCmd(binary) {decode encode format scan}
set {::subCmd(binary decode)} {base64 hex uuencode}
set {::subCmd(binary encode)} {base64 hex uuencode}
set ::subCmd(chan) {blocked close configure copy create eof event flush gets isbinary names pending pipe pop postevent push puts read seek tell truncate}
set ::subCmd(clock) {add clicks format microseconds milliseconds scan seconds}
set ::subCmd(dict) {append create exists filter for get getdef getwithdefault incr info keys lappend map merge remove replace set size unset update values with}
set ::subCmd(encoding) {convertfrom convertto dirs names profiles system}
set ::subCmd(file) {atime attributes channels copy delete dirname executable exists extension home isdirectory isfile join link lstat mkdir mtime nativename normalize owned pathtype readable readlink rename rootname separator size split stat system tail tempdir tempfile tildeexpand type volumes writable}
set ::subCmd(info) {args body class cmdcount cmdtype commands complete constant consts coroutine default errorstack exists frame functions globals hostname level library loaded locals nameofexecutable object patchlevel procs script sharedlibextension tclversion vars}
set {::subCmd(info class)} {call constructor definition definitionnamespace destructor filters forward instances methods methodtype mixins properties subclasses superclasses variables}
set {::subCmd(info object)} {call class creationid definition filters forward isa methods methodtype mixins namespace properties variables vars}
set ::subCmd(interp) {alias aliases bgerror cancel children create debug delete eval exists expose hidden hide invokehidden issafe limit marktrusted recursionlimit share target transfer}
set ::subCmd(namespace) {children code current delete ensemble eval exists export forget import inscope origin parent path qualifiers tail unknown upvar which}
set ::subCmd(oo::class) {create destroy}
set ::subCmd(oo::object) {create destroy new}
set ::subCmd(package) {files forget ifneeded names prefer present provide require unknown vcompare versions vsatisfies}
set ::subCmd(self) {call caller class filter method namespace next object target}
set ::subCmd(string) {cat compare equal first index insert is last length map match range repeat replace reverse tolower totitle toupper trim trimleft trimright wordend wordstart}
set {::subCmd(string is)} {alnum alpha ascii boolean control dict digit double entier false graph integer list lower print punct space true upper wideinteger wordchar xdigit}
set ::subCmd(tcl::prefix) {all longest match}
set ::subCmd(tcl::process) {autopurge list purge status}
set ::subCmd(trace) {add info remove}
set {::subCmd(trace add)} {command execution variable}
set {::subCmd(trace add execution)} {enter enterstep leave leavestep}
set {::subCmd(trace info)} {command execution variable}
set {::subCmd(trace remove)} {command execution variable}
set ::subCmd(update) idletasks
set ::subCmd(zipfs) {canonical exists find info list lmkimg lmkzip mkimg mkkey mkzip mount mountdata root unmount}
set ::subCmd(zlib) {adler32 compress crc32 decompress deflate gunzip gzip inflate push stream}
set {::subCmd(zlib push)} {compress decompress deflate gunzip gzip inflate}
set {::subCmd(zlib stream)} {compress decompress deflate gunzip gzip inflate}

set {::option(binary decode base64)} -strict
set {::option(binary decode hex)} -strict
set {::option(binary decode uuencode)} -strict
set {::option(binary encode base64)} {-maxlen -wrapchar}
set {::option(binary encode uuencode)} {-maxlen -wrapchar}
set {::option(chan puts)} -nonewline
set {::option(clock clicks)} {-microseconds -milliseconds}
set {::option(clock format)} {-format -gmt -locale -timezone}
set {::option(clock scan)} {-base -format -gmt -locale -timezone -validate}
set {::option(encoding convertfrom)} {-failindex -profile}
set {::option(encoding convertto)} {-failindex -profile}
set ::option(exec) {-- -ignorestderr -keepnewline}
set ::option(fconfigure) {-blocking -buffering -buffersize -closemode -encoding -eofchar -error -handshake -inputmode -lasterror -mode -peername -pollinterval -profile -queue -sockname -sysbuffer -timeout -translation -ttycontrol -ttystatus -winsize -xchar}
set ::option(fcopy) {-command -size}
set {::option(file attributes)} {-group -owner -permissions}
set {::option(file copy)} {-- -force}
set {::option(file delete)} {-- -force}
set {::option(file link)} {-hard -symbolic}
set {::option(file rename)} {-- -force}
set ::option(glob) {-- -directory -join -nocomplain -path -tails -types}
set {::option(glob -directory)} 1
set {::option(glob -path)} 1
set {::option(glob -types)} 1
set {::option(interp cancel)} {-- -unwind}
set {::option(interp invokehidden)} {-- -global -namespace}
set {::option(interp invokehidden -namespace)} 1
set ::option(lsearch) {-all -ascii -bisect -decreasing -dictionary -exact -glob -increasing -index -inline -integer -nocase -not -real -regexp -sorted -start -stride -subindices}
set {::option(lsearch -index)} 1
set {::option(lsearch -start)} 1
set {::option(lsearch -stride)} 1
set ::option(lsort) {-ascii -command -decreasing -dictionary -increasing -index -indices -integer -nocase -real -stride -unique}
set {::option(lsort -command)} 1
set {::option(lsort -index)} 1
set {::option(lsort -stride)} 1
set {::option(namespace which)} {-variable -command}
set {::option(namespace which -variable)} v
set {::option(oo::class create::filter)} {-append -clear -set}
set {::option(oo::class create::mixin)} {-append -clear -set}
set {::option(oo::class create::superclass)} {-append -clear -set}
set {::option(oo::class create::variable)} {-append -clear -set}
set ::option(oo::define::filter) {-append -appendifnew -clear -prepend -remove -set}
set ::option(oo::define::mixin) {-append -appendifnew -clear -prepend -remove -set}
set ::option(oo::define::superclass) {-append -appendifnew -clear -prepend -remove -set}
set ::option(oo::define::variable) {-append -appendifnew -clear -prepend -remove -set}
set ::option(oo::objdefine::filter) {-append -appendifnew -clear -prepend -remove -set}
set ::option(oo::objdefine::mixin) {-append -appendifnew -clear -prepend -remove -set}
set ::option(oo::objdefine::variable) {-append -appendifnew -clear -prepend -remove -set}
set {::option(package require)} -exact
set ::option(puts) -nonewline
set ::option(regexp) {-- -about -all -expanded -indices -inline -line -lineanchor -linestop -nocase -start}
set {::option(regexp -start)} 1
set ::option(regsub) {-- -all -command -expanded -line -lineanchor -linestop -nocase -start}
set {::option(regsub -command)} 1
set {::option(regsub -start)} 1
set ::option(source) -encoding
set {::option(string compare)} {-length -nocase}
set {::option(string compare -length)} 1
set {::option(string equal)} {-length -nocase}
set {::option(string equal -length)} 1
set {::option(string is)} {-failindex -strict}
set {::option(string is -failindex)} n
set {::option(string map)} -nocase
set {::option(string match)} -nocase
set ::option(subst) {-nobackslashes -nocommands -novariables}
set ::option(switch) {-- -exact -glob -indexvar -matchvar -nocase -regexp}
set {::option(switch -indexvar)} n
set {::option(switch -matchvar)} n
set {::option(tcl::prefix match)} {-error -exact -message}
set {::option(tcl::prefix match -error)} x
set {::option(tcl::prefix match -message)} x
set {::option(tcl::process status)} {-- -wait}
set ::option(timerate) {-direct -calibrate -overhead}
set {::option(timerate -overhead)} 1
set ::option(unload) {-- -keeplibrary -nocomplain}
set ::option(unset) {-nocomplain --}
set ::option(vwait) {-- -all -extended -nofileevents -noidleevents -notimerevents -nowindowevents -readable -timeout -variable -writable}
set {::option(vwait -readable)} x
set {::option(vwait -timeout)} x
set {::option(vwait -variable)} n
set {::option(vwait -writable)} x
set {::option(zipfs list)} {-glob -regexp}
set {::option(zlib gunzip)} {-buffersize -headerVar}
set {::option(zlib gzip)} {-header -level}
set {::option(zlib push)} {-dictionary -header -level}
set {::option(zlib stream compress)} {-dictionary -level}
set {::option(zlib stream decompress)} -dictionary
set {::option(zlib stream deflate)} {-dictionary -level}
set {::option(zlib stream gzip)} {-header -level}
set {::option(zlib stream inflate)} -dictionary

set ::syntax(oo::configurable) {s x*}
set {::syntax(oo::configurable create)} {do=_stdclass_oo cn?}
set {::syntax(oo::configurable create::configurable)} x
set {::syntax(oo::configurable create::constructor)} dk
set {::syntax(oo::configurable create::deletemethod)} {x x*}
set {::syntax(oo::configurable create::destructor)} dd
set {::syntax(oo::configurable create::export)} {x x*}
set {::syntax(oo::configurable create::filter)} {o? x*}
set {::syntax(oo::configurable create::forward)} {x x x*}
set {::syntax(oo::configurable create::method)} dm
set {::syntax(oo::configurable create::mixin)} {o? di}
set {::syntax(oo::configurable create::renamemethod)} {x x}
set {::syntax(oo::configurable create::self)} x*
set {::syntax(oo::configurable create::superclass)} {o? di}
set {::syntax(oo::configurable create::unexport)} {x x*}
set {::syntax(oo::configurable create::variable)} {o? div*}

