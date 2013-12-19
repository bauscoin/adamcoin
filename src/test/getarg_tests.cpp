#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-ADZ");
    BOOST_CHECK(GetBoolArg("-ADZ"));
    BOOST_CHECK(GetBoolArg("-ADZ", false));
    BOOST_CHECK(GetBoolArg("-ADZ", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-ADZo"));
    BOOST_CHECK(!GetBoolArg("-ADZo", false));
    BOOST_CHECK(GetBoolArg("-ADZo", true));

    ResetArgs("-ADZ=0");
    BOOST_CHECK(!GetBoolArg("-ADZ"));
    BOOST_CHECK(!GetBoolArg("-ADZ", false));
    BOOST_CHECK(!GetBoolArg("-ADZ", true));

    ResetArgs("-ADZ=1");
    BOOST_CHECK(GetBoolArg("-ADZ"));
    BOOST_CHECK(GetBoolArg("-ADZ", false));
    BOOST_CHECK(GetBoolArg("-ADZ", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noADZ");
    BOOST_CHECK(!GetBoolArg("-ADZ"));
    BOOST_CHECK(!GetBoolArg("-ADZ", false));
    BOOST_CHECK(!GetBoolArg("-ADZ", true));

    ResetArgs("-noADZ=1");
    BOOST_CHECK(!GetBoolArg("-ADZ"));
    BOOST_CHECK(!GetBoolArg("-ADZ", false));
    BOOST_CHECK(!GetBoolArg("-ADZ", true));

    ResetArgs("-ADZ -noADZ");  // -ADZ should win
    BOOST_CHECK(GetBoolArg("-ADZ"));
    BOOST_CHECK(GetBoolArg("-ADZ", false));
    BOOST_CHECK(GetBoolArg("-ADZ", true));

    ResetArgs("-ADZ=1 -noADZ=1");  // -ADZ should win
    BOOST_CHECK(GetBoolArg("-ADZ"));
    BOOST_CHECK(GetBoolArg("-ADZ", false));
    BOOST_CHECK(GetBoolArg("-ADZ", true));

    ResetArgs("-ADZ=0 -noADZ=0");  // -ADZ should win
    BOOST_CHECK(!GetBoolArg("-ADZ"));
    BOOST_CHECK(!GetBoolArg("-ADZ", false));
    BOOST_CHECK(!GetBoolArg("-ADZ", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--ADZ=1");
    BOOST_CHECK(GetBoolArg("-ADZ"));
    BOOST_CHECK(GetBoolArg("-ADZ", false));
    BOOST_CHECK(GetBoolArg("-ADZ", true));

    ResetArgs("--noADZ=1");
    BOOST_CHECK(!GetBoolArg("-ADZ"));
    BOOST_CHECK(!GetBoolArg("-ADZ", false));
    BOOST_CHECK(!GetBoolArg("-ADZ", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-ADZ", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ADZ", "eleven"), "eleven");

    ResetArgs("-ADZ -bar");
    BOOST_CHECK_EQUAL(GetArg("-ADZ", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ADZ", "eleven"), "");

    ResetArgs("-ADZ=");
    BOOST_CHECK_EQUAL(GetArg("-ADZ", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ADZ", "eleven"), "");

    ResetArgs("-ADZ=11");
    BOOST_CHECK_EQUAL(GetArg("-ADZ", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-ADZ", "eleven"), "11");

    ResetArgs("-ADZ=eleven");
    BOOST_CHECK_EQUAL(GetArg("-ADZ", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-ADZ", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-ADZ", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-ADZ", 0), 0);

    ResetArgs("-ADZ -bar");
    BOOST_CHECK_EQUAL(GetArg("-ADZ", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-ADZ=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-ADZ", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-ADZ=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-ADZ", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--ADZ");
    BOOST_CHECK_EQUAL(GetBoolArg("-ADZ"), true);

    ResetArgs("--ADZ=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-ADZ", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noADZ");
    BOOST_CHECK(!GetBoolArg("-ADZ"));
    BOOST_CHECK(!GetBoolArg("-ADZ", true));
    BOOST_CHECK(!GetBoolArg("-ADZ", false));

    ResetArgs("-noADZ=1");
    BOOST_CHECK(!GetBoolArg("-ADZ"));
    BOOST_CHECK(!GetBoolArg("-ADZ", true));
    BOOST_CHECK(!GetBoolArg("-ADZ", false));

    ResetArgs("-noADZ=0");
    BOOST_CHECK(GetBoolArg("-ADZ"));
    BOOST_CHECK(GetBoolArg("-ADZ", true));
    BOOST_CHECK(GetBoolArg("-ADZ", false));

    ResetArgs("-ADZ --noADZ");
    BOOST_CHECK(GetBoolArg("-ADZ"));

    ResetArgs("-noADZ -ADZ"); // ADZ always wins:
    BOOST_CHECK(GetBoolArg("-ADZ"));
}

BOOST_AUTO_TEST_SUITE_END()
