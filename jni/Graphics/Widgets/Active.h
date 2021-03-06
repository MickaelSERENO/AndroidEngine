#ifndef DEF_ACTIVE
#define DEF_ACTIVE

#include "logger.h"
#include "Listeners/Listener.h"

/** \brief basic class for activation / selection system as Button or CheckBox*/
class Active
{
	public:
		/** \brief Active base constructor
		 * \param select tell if the object is selected
		 * \param active tell if the object is activated
		 * \param alwaysUpdateSelection tell if we should always call the updateSelection funtion
		 * \param alwaysUpdateActivation tell if we should always call the updateActivation function*/
		Active(bool select = false, bool active = false, bool alwaysUpdateSelection = true, bool alwaysUpdateActivation = true);

		/** \brief The update function. It checks if the system is being selected or being activated*/
		virtual void update();

		/** \brief update the selection. Call the function "selectIt" or "deselectIt" switch the case
		 * \return if the system is selected*/
		virtual bool updateSelection();

		/** \brief update the activation. Call the function "activeIt" or "disactiveIt" switch the case
		 * \return if the system is activated*/
		virtual bool updateActivation();

		/** \brief select the system. For example for a button, the selection is when we touch it */
		virtual void selectIt();

		/** \brief deselect the system.*/
		virtual void deselectIt();

		/** \brief active the system*/
		virtual void activeIt();

		/** \brief disactive the system*/
		virtual void disactiveIt();

		/** \brief function for knowing if the system is currently being activated or not
		 * If not surcharged, returns false*/
		virtual bool howActive();

		/** \brief function for know if the system is currently being disactivated or not
		 * If not surcharged, returns false*/
		virtual bool howDisactive();

		/** \brief function for knowing if the system is currently being selected or not
		 * If not surcharged, returns false*/
		virtual bool howSelect();

		/** \brief function for knowing if the system is currently being deselected or not
		 * If not surcharged, returns false*/
		virtual bool howDeselect();

		/** \brief set permanently the selection. The system will be always selected
		 * \param permamentSelection tell if the system is in a permanent selection or not*/
		void setPermanentSelected(bool permamentSelection);

		/** \brief set permanently the activation. The system will be always activated
		 * \param permamentActivation tell if the system is in a permanent activation or not*/
		void setPermanentActivated(bool permamentActivation);

		/** \brief tell if the system should always check if it is selected or not (and make the proper action associated with that)
		 * \param alwaysUpdateSelection always update the selection or not*/
		void setAlwaysUpdateSelection(bool alwaysUpdateSelection);

		/** \brief tell if the system should always check if it is activated or not (and make the proper action associated with that)
		 * \param alwaysUpdateActivation always update the activation or not*/
		void setAlwaysUpdateActivation(bool alwaysUpdateActivation);

		/** \brief setter for selectOnce. This variable is used for knowing if we should select only once before being deselected
		 * \param selectOnce the new value of selectOnce*/
		void setSelectOnce(bool selectOnce);

		/** \brief setter for activeOnce. This variable is used for knowing if we should active only once before being disactivated
		 * \param the new value of activeOnce*/
		void setActiveOnce(bool activeOnce);

		/** \brief the system is selected or not
		 * \return the system is selected or not*/
		bool isSelect() const;

		/** \brief the system is in a permanent selection or not
		 * \return the system is in a permanent selection or not*/
		bool isPermanentSelected() const;

		/** \brief the system will always check if it is selected or not (and make the proper action)
		  * \brief the system will always check if it is selected or not (and make the proper action)*/
		bool isAlwaysUpdateSelection() const;

		/** \brief the system is activated or not
		 * \return the system is activated or not*/
		bool isActive() const;

		/** \brief the system is in a permanent activation or not
		 * \return the system is in a permanent activation or not*/
		bool isPermanentActivated() const;

		/** \brief the system will always check if it is activated or not (and make the proper action)
		  * \brief the system will always check if it is activated or not (and make the proper action)*/
		bool isAlwaysUpdateActivation() const;

		/** \brief the system will call selectIt only once before being deselected
		 * \return if the system is selected only once before being deselected*/
		bool isSelectOnce() const;

		/** \brief the system will call activeIt only once before being disactivated
		 * \return if the system is selected only once before being disactivated*/
		bool isActiveOnce() const;

		/** \brief The active function is the function called when the system is activated
		 * \param fireActive The ActiveListener which will be called*/
		void setActiveListener(const ActiveListener& fireActive);

		/** \brief The disactive function is the function called when the system is disactivated
		 * \param fireDisactive The ActiveListener which will be called*/
		void setDisactiveListener(const ActiveListener& fireDisactive);

		/** \brief The change function is the function called when the Active object change its state
		 * \param fireChange The ActiveListener which will be called*/
		void setOnChangeListener(const ActiveListener& fireChange);
	protected:
		bool m_isSelect;
		bool m_isActive;
		bool m_alwaysUpdateSelection;
		bool m_alwaysUpdateActivation;
		bool m_permanentSelected;
		bool m_permanentActivated;
		bool m_selectOnce;
		bool m_activeOnce;

		ActiveListener m_fireActive;
		ActiveListener m_fireDisactive;
		ActiveListener m_fireChange;
};

#endif
