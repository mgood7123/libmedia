class Automakeffmpeg < Formula
    desc "Tool for generating GNU Standards-compliant Makefiles"
    homepage "https://www.gnu.org/software/automake/"
    url "http://ftpmirror.gnu.org/automake/automake-1.15.1.tar.xz"
    mirror "https://ftp.gnu.org/gnu/automake/automake-1.15.1.tar.xz"
    sha256 "af6ba39142220687c500f79b4aa2f181d9b24e4f8d8ec497cea4ba26c64bedaf"

    depends_on "autoconf" => :run

    def install
      ENV["PERL"] = "/usr/bin/perl"

      system "./configure", "--prefix=#{prefix}"
      system "make", "install"

      # Our aclocal must go first. See:
      # https://github.com/Homebrew/homebrew/issues/10618
      (share/"aclocal/dirlist").write <<-EOS
        #{HOMEBREW_PREFIX}/share/aclocal
        /usr/share/aclocal
      EOS
    end

    test do
        (testpath/"test.c").write <<~EOS
          int main() { return 0; }
        EOS
        (testpath/"configure.ac").write <<~EOS
          AC_INIT(test, 1.0)
          AM_INIT_AUTOMAKE
          AC_PROG_CC
          AC_CONFIG_FILES(Makefile)
          AC_OUTPUT
        EOS
        (testpath/"Makefile.am").write <<~EOS
          bin_PROGRAMS = test
          test_SOURCES = test.c
        EOS
        system bin/"aclocal"
        system bin/"automake", "--add-missing", "--foreign"
        system "autoconf"
        system "./configure"
        system "make"
        system "./test"
      end
end