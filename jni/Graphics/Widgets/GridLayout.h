#ifndef  GRIDLAYOUT_INC
#define  GRIDLAYOUT_INC

#include "GroupTransformable.h"
#include "Updatable.h"
#include "Drawable.h"
#include "Vector2.h"

/** \class GridLayout
 * \brief Permit to place widgets on the GridLayout. The coordinate system is the same as opengl (0,0 is at the bottom left)*/
class GridLayout : public Updatable, public GroupTransformable
{
	public:
		GridLayout(Updatable* parent);

		/** \brief Add a widget to the GridLayout given a position
		 * \param drawable The Drawable to add
		 * \param x the x coordinate of the GridLayout
		 * \param y the y coordinate of the GridLayout
		 * \param nbXCases the number of columns the widget takes
		 * \param nbYCases the number of rows the widget takes*/
		void addWidget(Drawable* drawable, uint32_t x, uint32_t y, uint32_t nbXCases=1, uint32_t nbYCases=1);

		/** \brief Remove the widget from the GridLayout given the Widget
		 * \param drawable The Widget to remove*/
		void removeWidget(Drawable* drawable);

		/** \brief Remove the widget from the GridLayout given a position
		 * \param drawable The Widget to remove*/
		void removeWidget(uint32_t x, uint32_t y);
	private:
		/** \brief Replace correctly the widget on the GridLayout*/
		void resetPosition();

		std::vector<std::vector<Drawable*>> m_widgets;/* <!All the widgets */
		std::vector<std::vector<Vector2ui>> m_widgetsSize; /* <!All the widgets case taken */
};

#endif