#include <gtest/gtest.h>
#include "all.h"

/* Set up a given item as a spell scroll, allowing for its usage. */
void set_up_scroll(dvl::ItemStruct &item, dvl::spell_id spell)
{
	dvl::pcurs = dvl::CURSOR_HAND;
	dvl::leveltype = dvl::DTYPE_CATACOMBS;
	dvl::plr[dvl::myplr]._pRSpell = static_cast<dvl::spell_id>(spell);
	item._itype = dvl::ITYPE_MISC;
	item._iMiscId = dvl::IMISC_SCROLL;
	item._iSpell = spell;
}

// Test that the scroll is used in the inventory in correct conditions
TEST(Inv, UseScroll_from_inventory)
{
	set_up_scroll(dvl::plr[dvl::myplr].InvList[2], dvl::SPL_FIREBOLT);
	dvl::plr[dvl::myplr]._pNumInv = 5;
	EXPECT_TRUE(dvl::UseScroll());
}

// Test that the scroll is used in the belt in correct conditions
TEST(Inv, UseScroll_from_belt)
{
	set_up_scroll(dvl::plr[dvl::myplr].SpdList[2], dvl::SPL_FIREBOLT);
	EXPECT_TRUE(dvl::UseScroll());
}

// Test that the scroll is not used in the inventory for each invalid condition
TEST(Inv, UseScroll_from_inventory_invalid_conditions)
{
	// Empty the belt to prevent using a scroll from the belt
	for (int i = 0; i < MAXBELTITEMS; i++) {
		dvl::plr[dvl::myplr].SpdList[i]._itype = dvl::ITYPE_NONE;
	}

	set_up_scroll(dvl::plr[dvl::myplr].InvList[2], dvl::SPL_FIREBOLT);
	dvl::pcurs = dvl::CURSOR_IDENTIFY;
	EXPECT_FALSE(dvl::UseScroll());

	set_up_scroll(dvl::plr[dvl::myplr].InvList[2], dvl::SPL_FIREBOLT);
	dvl::leveltype = dvl::DTYPE_TOWN;
	EXPECT_FALSE(dvl::UseScroll());

	set_up_scroll(dvl::plr[dvl::myplr].InvList[2], dvl::SPL_FIREBOLT);
	dvl::plr[dvl::myplr]._pRSpell = static_cast<dvl::spell_id>(dvl::SPL_HEAL);
	EXPECT_FALSE(dvl::UseScroll());

	set_up_scroll(dvl::plr[dvl::myplr].InvList[2], dvl::SPL_FIREBOLT);
	dvl::plr[dvl::myplr].InvList[2]._iMiscId = dvl::IMISC_STAFF;
	EXPECT_FALSE(dvl::UseScroll());

	set_up_scroll(dvl::plr[dvl::myplr].InvList[2], dvl::SPL_FIREBOLT);
	dvl::plr[dvl::myplr].InvList[2]._itype = dvl::ITYPE_NONE;
	EXPECT_FALSE(dvl::UseScroll());
}

// Test that the scroll is not used in the belt for each invalid condition
TEST(Inv, UseScroll_from_belt_invalid_conditions)
{
	// Disable the inventory to prevent using a scroll from the inventory
	dvl::plr[dvl::myplr]._pNumInv = 0;

	set_up_scroll(dvl::plr[dvl::myplr].SpdList[2], dvl::SPL_FIREBOLT);
	dvl::pcurs = dvl::CURSOR_IDENTIFY;
	EXPECT_FALSE(dvl::UseScroll());

	set_up_scroll(dvl::plr[dvl::myplr].SpdList[2], dvl::SPL_FIREBOLT);
	dvl::leveltype = dvl::DTYPE_TOWN;
	EXPECT_FALSE(dvl::UseScroll());

	set_up_scroll(dvl::plr[dvl::myplr].SpdList[2], dvl::SPL_FIREBOLT);
	dvl::plr[dvl::myplr]._pRSpell = static_cast<dvl::spell_id>(dvl::SPL_HEAL);
	EXPECT_FALSE(dvl::UseScroll());

	set_up_scroll(dvl::plr[dvl::myplr].SpdList[2], dvl::SPL_FIREBOLT);
	dvl::plr[dvl::myplr].SpdList[2]._iMiscId = dvl::IMISC_STAFF;
	EXPECT_FALSE(dvl::UseScroll());

	set_up_scroll(dvl::plr[dvl::myplr].SpdList[2], dvl::SPL_FIREBOLT);
	dvl::plr[dvl::myplr].SpdList[2]._itype = dvl::ITYPE_NONE;
	EXPECT_FALSE(dvl::UseScroll());
}
